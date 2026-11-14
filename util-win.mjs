import fs from 'node:fs';
import path from 'node:path';

const features = {
  avif: /^(lib)?avif/,
  heif: /^(lib)?heif/,
  tiff: /^(lib)?tiff/,
  xpm: /^(lib)?xpm/,
  jpeg: /^(lib)?(turbo)?jpeg/,
  fontconfig: /^(lib)?fontconfig/,
  freetype: /^(lib)?freetype/,
  png16: /^(lib)?png/,
  webp: /^(lib)?webp$/,
  vpx: /^(lib)?vpx/,
};

function fail(message) {
  process.stderr.write(`node-gd: ${message}\n`);
  process.exit(1);
}

function findPrefix(arch) {
  if (process.env.GDLIB_PREFIX) {
    return path.resolve(process.env.GDLIB_PREFIX);
  }
  if (process.env.VCPKG_ROOT) {
    return path.join(process.env.VCPKG_ROOT, 'installed', `${arch}-windows-static`);
  }
  fail(
    'set GDLIB_PREFIX to the directory with include\\gd.h and lib\\pkgconfig\\gdlib.pc, ' +
      `or VCPKG_ROOT to a vcpkg root with libgd installed for ${arch}-windows-static`
  );
}

function readPc(dir, name) {
  const file = path.join(dir, `${name}.pc`);
  if (!fs.existsSync(file)) {
    fail(`${file} not found`);
  }
  const variables = { pcfiledir: dir.replaceAll('\\', '/') };
  const fields = {};
  const expand = (value) => value.replace(/\$\{(\w+)\}/g, (match, key) => variables[key] ?? '');
  for (const line of fs.readFileSync(file, 'utf8').split(/\r?\n/)) {
    const field = line.match(/^([\w.]+):\s*(.*)$/);
    const variable = line.match(/^(\w+)=(.*)$/);
    if (field) {
      fields[field[1].toLowerCase()] = expand(field[2]).trim();
    } else if (variable) {
      variables[variable[1]] = expand(variable[2]).trim();
    }
  }
  return fields;
}

function packageNames(requires = '') {
  return requires
    .replace(/[<>=!]+\s*\S+/g, '')
    .split(/[\s,]+/)
    .filter(Boolean);
}

function resolve(prefix) {
  const pcDir = path.join(prefix, 'lib', 'pkgconfig');
  const packages = [];
  const libraries = [];
  const visit = (name) => {
    if (packages.includes(name)) {
      return;
    }
    packages.push(name);
    const pc = readPc(pcDir, name);
    for (const token of `${pc.libs ?? ''} ${pc['libs.private'] ?? ''}`.split(/\s+/)) {
      const library = token.startsWith('-l') ? `${token.slice(2)}.lib` : token.endsWith('.lib') ? path.basename(token) : null;
      if (library && !libraries.includes(library)) {
        libraries.push(library);
      }
    }
    for (const required of packageNames(`${pc.requires ?? ''} ${pc['requires.private'] ?? ''}`)) {
      visit(required);
    }
  };
  visit('gdlib');
  return { packages, libraries };
}

const [arch, command, argument] = process.argv.slice(2);
const prefix = findPrefix(arch);

switch (command) {
  case 'include':
    if (!fs.existsSync(path.join(prefix, 'include', 'gd.h'))) {
      fail(`${path.join(prefix, 'include', 'gd.h')} not found`);
    }
    console.log(path.join(prefix, 'include'));
    break;
  case 'libdir':
    console.log(path.join(prefix, 'lib'));
    break;
  case 'libraries':
    console.log(resolve(prefix).libraries.join(' '));
    break;
  case 'feature': {
    const pattern = features[argument];
    if (!pattern) {
      fail(`unknown feature ${argument}`);
    }
    const { packages, libraries } = resolve(prefix);
    const names = [...packages, ...libraries.map((library) => library.replace(/\.lib$/, ''))];
    console.log(names.some((name) => pattern.test(name.toLowerCase())) ? 'true' : 'false');
    break;
  }
  default:
    fail(`unknown command ${command}`);
}
