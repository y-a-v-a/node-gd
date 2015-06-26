{
  "conditions": [
    ['OS!="win"', {
      'variables': {
        'with_tiff%': '<!(./util.sh GD_TIFF)',
        'with_xpm%': '<!(./util.sh GD_XPM)',
        'with_jpeg%': '<!(./util.sh GD_JPEG)',
        'with_fontconfig%': '<!(./util.sh GD_FONTCONFIG)',
        'with_freetype%': '<!(./util.sh GD_FREETYPE)',
        'with_png%': '<!(./util.sh GD_PNG)',
        'with_webp%': '<!(./util.sh GD_WEBP)',
        'with_vpx%': '<!(./util.sh GD_VPX)'
      }
    }]
  ],
  "targets": [
    {
      "target_name": "node_gd",
      "sources": ["cpp/node-gd.cpp"],
      "libraries": ["-lgd"],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "conditions": [
        [ "OS=='freebsd'", {
          "libraries": ["-L/usr/local/lib"],
          "include_dirs": ["/usr/local/include"]
        }],
        [ "OS=='mac'", {
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
