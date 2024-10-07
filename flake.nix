{
  description = "A simple do-nothing shared object";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";

  outputs = {
    self,
    nixpkgs,
  }: let
    supportedSystems = ["x86_64-linux" "aarch64-linux"];
    forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
    nixpkgsFor = forAllSystems (system:
      import nixpkgs {
        inherit system;
        overlays = [
          self.overlays.default
        ];
      });
  in {
    overlays.default = final: prev: {
      donothing = prev.callPackage ./derivation.nix {};
    };

    formatter = forAllSystems (system: (nixpkgsFor.${system}).alejandra);

    packages = forAllSystems (system: {
      default = (nixpkgsFor.${system}).donothing;
    });
  };
}
