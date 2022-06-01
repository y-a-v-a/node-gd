import path from 'path';
import { fileURLToPath } from 'url';

const dirname = (url) => {
  const filePath = fileURLToPath(url);
  const dirname = path.dirname(filePath);
  return dirname;
};

export default dirname;
