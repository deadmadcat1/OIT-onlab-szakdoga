{
	description = "Szakdoga környezet flake";
	inputs = {
		nixpkgs.url = "nixpkgs/nixos-25.11";
	};
	outputs = {nixpkgs, ...}:
	let
		system = "x86_64-linux";
		pkgs = import nixpkgs { inherit system; };
	in
	{
		devShells.${system}.default = pkgs.mkShell {
			packages = with pkgs; [
				gnumake
				cmake
				gcc

				glfw
				glew
				glm

				clang-tools
				lldb
				glsl_analyzer
				renderdoc
			];
			env = {
				GLFW_LIB = "${pkgs.glfw}";
				GLEW_LIB = "${pkgs.glew.dev}";
				GLEW_OUT = "${pkgs.glew.out}";
				GLM_LIB = "${pkgs.glm}";
			};
		};
	};
}
