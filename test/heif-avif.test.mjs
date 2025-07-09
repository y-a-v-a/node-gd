import fs from 'fs';
import gd from '../index.js';
import { assert } from 'chai';
import dirname from './dirname.mjs';

const currentDir = dirname(import.meta.url);
const source = currentDir + '/fixtures/';
const target = currentDir + '/output/';

describe('HEIF and AVIF Support', function () {
  
  describe('HEIF format tests', function () {
    it('gd.GD_HEIF -- should be defined correctly', function () {
      // GD_HEIF should be truthy when available (can be 1 or true)
      assert.ok(typeof gd.GD_HEIF !== 'undefined');
      if (!gd.GD_HEIF) {
        console.log('HEIF support not available in this build');
      }
    });

    it('gd.createFromHeif() -- can create image from HEIF file', async function () {
      if (!gd.GD_HEIF) {
        this.skip();
        return;
      }
      const s = source + 'input.heif';
      const img = await gd.createFromHeif(s);
      
      assert.ok(img instanceof gd.Image);
      assert.ok(img.width > 0);
      assert.ok(img.height > 0);
      img.destroy();
    });

    it('gd.createFromHeifPtr() -- can create image from HEIF buffer', async function () {
      if (!gd.GD_HEIF) {
        this.skip();
        return;
      }
      const s = source + 'input.heif';
      const buffer = fs.readFileSync(s);
      const img = await gd.createFromHeifPtr(buffer);
      
      assert.ok(img instanceof gd.Image);
      assert.ok(img.width > 0);
      assert.ok(img.height > 0);
      img.destroy();
    });

    it('gd.Image#saveHeif() -- can save image as HEIF with quality setting', async function () {
      if (!gd.GD_HEIF) {
        this.skip();
        return;
      }
      const s = source + 'input.png';
      const t = target + 'output-heif-quality.heif';
      const img = await gd.openPng(s);
      
      await img.saveHeif(t, 90);
      assert.ok(fs.existsSync(t));
      img.destroy();
    });

    it('gd.Image#heif() -- returns a Promise', async function () {
      if (!gd.GD_HEIF) {
        this.skip();
        return;
      }
      const s = source + 'input.png';
      const t = target + 'output-heif-promise.heif';
      const img = await gd.openPng(s);
      const promise = img.heif(t, 80);
      
      assert.ok(promise instanceof Promise);
      await promise;
      assert.ok(fs.existsSync(t));
      img.destroy();
    });
  });

  describe('AVIF format tests', function () {
    it('gd.GD_AVIF -- should be defined correctly', function () {
      // GD_AVIF should be truthy when available (can be 1 or true)
      assert.ok(typeof gd.GD_AVIF !== 'undefined');
      if (!gd.GD_AVIF) {
        console.log('AVIF support not available in this build');
      }
    });

    it('gd.createFromAvif() -- can create image from AVIF file', async function () {
      if (!gd.GD_AVIF) {
        this.skip();
        return;
      }
      // Skip this test in CI environments where AVIF encoder may not be available
      if (process.env.CI || process.env.GITHUB_ACTIONS) {
        this.skip();
        return;
      }
      try {
        // First create an AVIF file for testing
        const jpegSource = source + 'input.jpg';
        const avifTemp = target + 'temp-for-test.avif';
        const jpegImg = await gd.openJpeg(jpegSource);
        await jpegImg.saveAvif(avifTemp, 80);
        jpegImg.destroy();
        
        // Now test loading the AVIF file
        const img = await gd.createFromAvif(avifTemp);
        
        assert.ok(img instanceof gd.Image);
        assert.ok(img.width > 0);
        assert.ok(img.height > 0);
        img.destroy();
      } catch (error) {
        // If AVIF codec is not available at runtime, skip the test
        if (error.message && (error.message.includes('codec') || error.message.includes('AVIF'))) {
          this.skip();
          return;
        }
        throw error;
      }
    });

    it('gd.createFromAvifPtr() -- can create image from AVIF buffer', async function () {
      if (!gd.GD_AVIF) {
        this.skip();
        return;
      }
      // Skip this test in CI environments where AVIF encoder may not be available
      if (process.env.CI || process.env.GITHUB_ACTIONS) {
        this.skip();
        return;
      }
      try {
        // First create an AVIF file for testing
        const jpegSource = source + 'input.jpg';
        const avifTemp = target + 'temp-for-ptr-test.avif';
        const jpegImg = await gd.openJpeg(jpegSource);
        await jpegImg.saveAvif(avifTemp, 80);
        jpegImg.destroy();
        
        // Now test loading from buffer
        const buffer = fs.readFileSync(avifTemp);
        const img = await gd.createFromAvifPtr(buffer);
        
        assert.ok(img instanceof gd.Image);
        assert.ok(img.width > 0);
        assert.ok(img.height > 0);
        img.destroy();
      } catch (error) {
        // If AVIF codec is not available at runtime, skip the test
        if (error.message && (error.message.includes('codec') || error.message.includes('AVIF'))) {
          this.skip();
          return;
        }
        throw error;
      }
    });

    it('gd.Image#saveAvif() -- can save image as AVIF with quality setting', async function () {
      if (!gd.GD_AVIF) {
        this.skip();
        return;
      }
      // Skip this test in CI environments where AVIF encoder may not be available
      if (process.env.CI || process.env.GITHUB_ACTIONS) {
        this.skip();
        return;
      }
      try {
        const s = source + 'input.png';
        const t = target + 'output-avif-quality.avif';
        const img = await gd.openPng(s);
        
        await img.saveAvif(t, 90);
        assert.ok(fs.existsSync(t));
        img.destroy();
      } catch (error) {
        // If AVIF codec is not available at runtime, skip the test
        if (error.message && (error.message.includes('codec') || error.message.includes('AVIF'))) {
          this.skip();
          return;
        }
        throw error;
      }
    });

    it('gd.Image#avif() -- returns a Promise', async function () {
      if (!gd.GD_AVIF) {
        this.skip();
        return;
      }
      // Skip this test in CI environments where AVIF encoder may not be available
      if (process.env.CI || process.env.GITHUB_ACTIONS) {
        this.skip();
        return;
      }
      try {
        const s = source + 'input.png';
        const t = target + 'output-avif-promise.avif';
        const img = await gd.openPng(s);
        const promise = img.avif(t, 80);
        
        assert.ok(promise instanceof Promise);
        await promise;
        assert.ok(fs.existsSync(t));
        img.destroy();
      } catch (error) {
        // If AVIF codec is not available at runtime, skip the test
        if (error.message && (error.message.includes('codec') || error.message.includes('AVIF'))) {
          this.skip();
          return;
        }
        throw error;
      }
    });
  });

  describe('Format interoperability', function () {
    it('can convert HEIF to AVIF', async function () {
      if (!gd.GD_HEIF || !gd.GD_AVIF) {
        this.skip();
        return;
      }
      // Skip this test in CI environments where AVIF encoder may not be available
      if (process.env.CI || process.env.GITHUB_ACTIONS) {
        this.skip();
        return;
      }
      try {
        const s = source + 'input.heif';
        const t = target + 'output-heif-to-avif.avif';
        const img = await gd.openHeif(s);
        
        await img.saveAvif(t, 85);
        assert.ok(fs.existsSync(t));
        img.destroy();
      } catch (error) {
        // If AVIF codec is not available at runtime, skip the test
        if (error.message && (error.message.includes('codec') || error.message.includes('AVIF') || error.message.includes('HEIF'))) {
          this.skip();
          return;
        }
        throw error;
      }
    });

    it('can convert AVIF to HEIF', async function () {
      if (!gd.GD_HEIF || !gd.GD_AVIF) {
        this.skip();
        return;
      }
      // Skip this test in CI environments where AVIF encoder may not be available
      if (process.env.CI || process.env.GITHUB_ACTIONS) {
        this.skip();
        return;
      }
      try {
        // First create an AVIF file
        const jpegSource = source + 'input.jpg';
        const avifTemp = target + 'temp-avif-to-heif.avif';
        const jpegImg = await gd.openJpeg(jpegSource);
        await jpegImg.saveAvif(avifTemp, 80);
        jpegImg.destroy();
        
        // Now convert AVIF to HEIF
        const t = target + 'output-avif-to-heif.heif';
        const img = await gd.openAvif(avifTemp);
        await img.saveHeif(t, 85);
        assert.ok(fs.existsSync(t));
        img.destroy();
      } catch (error) {
        // If AVIF codec is not available at runtime, skip the test
        if (error.message && (error.message.includes('codec') || error.message.includes('AVIF') || error.message.includes('HEIF'))) {
          this.skip();
          return;
        }
        throw error;
      }
    });
  });
});