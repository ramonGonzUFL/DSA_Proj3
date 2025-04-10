{
  inputs = {
    utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, nixpkgs, utils }: utils.lib.eachDefaultSystem (system:
    let
      pkgs = nixpkgs.legacyPackages.${system};
    in
    {
      devShell = pkgs.mkShell {
	nativeBuildInputs = with pkgs; [
          cmake
          pkg-config
          clang-tools
        ];
        buildInputs = with pkgs; [
          # sfml # Uses git based in cmake. Switch to this later? or keep the git based for other partners
          xorg.libXrandr
          xorg.libXcursor
          xorg.libXi
          udev
          flac
          libvorbis
          libGL
          freetype
        ];
      };
    }
  );
}
