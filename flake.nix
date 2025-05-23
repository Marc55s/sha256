{
    description = "C Development Shell with Clang and LSP support";

    inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

    outputs = { self, nixpkgs }: 
        let 
            pkgs = import nixpkgs { system = "x86_64-linux"; };
            system = "x86_64-linux";  # Adjust for your system architecture
        in {
            devShells.x86_64-linux.default = pkgs.mkShell {
                buildInputs = [
                    pkgs.clang_19           # C compiler
                    pkgs.clang-tools     # LSP support (includes clangd)
                    pkgs.glibc.dev
                    pkgs.glibc
                    pkgs.gnumake         # Make tool
                    pkgs.pkg-config      # Dependency management
                    pkgs.libclang        # Required for some LSP features
                ];

                shellHook = ''
                    export CC=clang
                    export CXX=clang++
                    export PATH=$PATH:$PWD/bin
                    export CL="${pkgs.clang_19}"
                    export CFLAGS="-I${pkgs.glibc.dev}/include -I${pkgs.clang_19}/resource-root/include"
                '';
            };

            # Define a build routine (if applicable)
            defaultPackage.${system} = pkgs.stdenv.mkDerivation {
                pname = "sha256";
                version = "1.0.0";

                src = self;  # Use the current directory for your source code

                # Optional: Environment variables for Raylib paths
                shellHook = ''
                    export CC=clang
                    export CXX=clang++
                    export PATH=$PATH:$PWD/bin
                    export CL="${pkgs.clang_19}"
                    export CFLAGS="-I${pkgs.glibc.dev}/include -I${pkgs.clang_19}/resource-root/include"
                '';

                buildInputs = [
                    pkgs.clang_19           # C compiler
                    pkgs.clang-tools     # LSP support (includes clangd)
                    pkgs.glibc.dev
                    pkgs.glibc
                    pkgs.gnumake         # Make tool
                    pkgs.pkg-config      # Dependency management
                    pkgs.libclang        # Required for some LSP features
                ];

                buildPhase = ''
                    echo "Building..."
                        make
                    '';
                installPhase = ''
                mkdir -p $out/bin
                cp sha256 $out/bin/
                '';
            };

            # Define a run routine (if applicable)
            defaultApp.${system} = {
                type = "app";
                program = "${self.defaultPackage.${system}}/bin/sha256";
            };
        };
}

