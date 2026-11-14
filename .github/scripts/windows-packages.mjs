import { execSync } from 'node:child_process';
import fs from 'node:fs';
import path from 'node:path';

const [prebuildsArg, outArg, mode = 'version'] = process.argv.slice(2);
if (!prebuildsArg || !outArg || !['version', 'tarball'].includes(mode)) {
  console.error('Usage: node windows-packages.mjs <prebuilds dir> <output dir> [version|tarball]');
  process.exit(1);
}

const rootDir = path.resolve(import.meta.dirname, '..', '..');
const prebuildsDir = path.resolve(prebuildsArg);
const outDir = path.resolve(outArg);
const rootFile = path.join(rootDir, 'package.json');
const root = JSON.parse(fs.readFileSync(rootFile, 'utf8'));

const targets = fs
  .readdirSync(prebuildsDir)
  .filter((name) => name.startsWith('win32-'))
  .map((name) => ({ name, arch: name.slice('win32-'.length) }));

if (targets.length === 0) {
  console.error(`No win32-* prebuilds in ${prebuildsDir}`);
  process.exit(1);
}

root.optionalDependencies = {};

for (const { name, arch } of targets) {
  const source = path.join(prebuildsDir, name, 'node-gd.node');
  const packageDir = path.join(outDir, name);
  const packageName = `@y-a-v-a/node-gd-${name}`;
  fs.mkdirSync(packageDir, { recursive: true });
  fs.copyFileSync(source, path.join(packageDir, 'node-gd.node'));
  fs.copyFileSync(path.join(rootDir, 'LICENSE'), path.join(packageDir, 'LICENSE'));
  fs.writeFileSync(
    path.join(packageDir, 'README.md'),
    `# ${packageName}\n\nThe Windows ${arch} binary of [node-gd](https://www.npmjs.com/package/node-gd). Install \`node-gd\` instead, it depends on this package.\n`
  );
  fs.writeFileSync(
    path.join(packageDir, 'package.json'),
    JSON.stringify(
      {
        name: packageName,
        version: root.version,
        description: `Windows ${arch} binary of node-gd, with libgd and its libraries linked in`,
        license: root.license,
        repository: root.repository,
        homepage: root.homepage,
        engines: root.engines,
        os: ['win32'],
        cpu: [arch],
        main: 'node-gd.node',
        files: ['node-gd.node'],
      },
      null,
      2
    ) + '\n'
  );

  if (mode === 'tarball') {
    const result = JSON.parse(execSync(`npm pack --json --pack-destination "${outDir}"`, { cwd: packageDir, encoding: 'utf8' }));
    const packed = Array.isArray(result) ? result[0] : result[packageName];
    root.optionalDependencies[packageName] = `file:${path.join(outDir, packed.filename)}`;
  } else {
    root.optionalDependencies[packageName] = root.version;
  }
  fs.rmSync(path.join(prebuildsDir, name), { recursive: true });
  console.log(`${packageName}@${root.version}: ${path.relative(rootDir, packageDir) || packageDir}`);
}

fs.writeFileSync(rootFile, JSON.stringify(root, null, 2) + '\n');
console.log('optionalDependencies:', root.optionalDependencies);
