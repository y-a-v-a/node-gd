{
  "conditions": [
    ['OS!="win"', {
      'variables': {
        'with_tiff%': '<!(./util.sh tiff)',
        'with_xpm%': '<!(./util.sh xpm)',
        'with_jpeg%': '<!(./util.sh jpeg)',
        'with_fontconfig%': '<!(./util.sh fontconfig)',
        'with_freetype%': '<!(./util.sh freetype)',
        'with_png%': '<!(./util.sh png16)',
        'with_webp%': '<!(./util.sh webp)',
        'with_vpx%': '<!(./util.sh vpx)'
      }
    }]
  ],
  "targets": [
    {
      "target_name": "node_gd",
      "sources": [
        "src/addon.cc"
      ],
      "libraries": ["-lgd"],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'defines': [ 'NAPI_CPP_EXCEPTIONS' ],
      "conditions": [
        [ "OS=='linux'", {
          "cflags!": [ '-fno-exceptions' ],
          "cflags_cc!": [ "-fno-exceptions" ]
        }],
        [ "OS=='freebsd'", {
          "libraries": ["-L/usr/local/lib"],
          "include_dirs": ["/usr/local/include"],
          "cflags!": [ '-fno-exceptions' ],
          "cflags_cc!": [ "-fno-exceptions" ]
        }],
        [ "OS=='mac'", {
          'cflags+': ['-fvisibility=hidden'],
          'cflags!': [ '-fno-exceptions' ],
          'cflags_cc!': [ '-fno-exceptions' ],
          'xcode_settings': {
            'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES', # -fvisibility=hidden
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
          },
          "libraries": ["-L/usr/local/lib", "-L/opt/local/lib"],
          "include_dirs": ["/usr/local/include", "/opt/local/include"]
        }],
        ["with_tiff=='true'", {
          'defines': [
            'HAVE_LIBTIFF',
            'GD_TIFF'
          ]
        }],
        ["with_xpm=='true'", {
          'defines': [
            'HAVE_LIBXPM',
            'GD_XPM'
          ]
        }],
        ["with_jpeg=='true'", {
          'defines': [
            'HAVE_LIBJPEG',
            'GD_JPEG'
          ]
        }],
        ["with_fontconfig=='true'", {
          'defines': [
            'HAVE_LIBFONTCONFIG',
            'GD_FONTCONFIG'
          ]
        }],
        ["with_freetype=='true'", {
          'defines': [
            'HAVE_LIBFREETYPE',
            'GD_FREETYPE'
          ]
        }],
        ["with_png=='true'", {
          'defines': [
            'HAVE_LIBPNG',
            'GD_PNG'
          ]
        }],
        ["with_webp=='true'", {
          'defines': [
            'HAVE_LIBWEBP',
            'GD_WEBP'
          ]
        }],
        ["with_vpx=='true'", {
          'defines': [
            'HAVE_LIBVPX',
            'GD_VPX'
          ]
        }]
      ]
    }
  ]
}
