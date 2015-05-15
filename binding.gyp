{
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
        }]
      ]
    }
  ]
}
