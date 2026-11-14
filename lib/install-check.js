if (process.env.npm_config_build_from_source === 'true') {
  process.exit(1);
}

try {
  await import('./bindings.js');
} catch (error) {
  console.error(`${error.message}\nnode-gd: building from source`);
  process.exit(1);
}
