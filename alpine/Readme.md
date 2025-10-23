All the files in this directry, including, the rest of the Readme, are copy from v86
repository, andmodify to build a image for cringy that can be upload to itch.io

You can build a Alpine Linux 9p image using Docker:

1. As needed, kernel flavor (`virt` is smaller than `lts`) and set of additional packages (community repo is enabled by default) can be edited in `Dockerfile`
2. Check and run `./build.sh` with started dockerd (podman works)
3. Run local webserver (e.g. `make run`) and open `examples/alpine.html`
4. (optional) Run `./build-state.js` and add `initial_state: { url: "../images/alpine-state.bin" }` to `alpine.html`
