(function (root, factory) {
    module.exports = factory(require("fs"), require("path"), require('child_process').spawnSync);
})(this, function (fs, path, cmd) {
    function BuildTool() {
        this.arch = (function () {
            if (process.arch === "ia32") {
                return "x86";
            } else if (process.arch === "x64") {
                return process.arch;
            }

            return "Unknown";
        })();

        this.isWindows = function () {
            return process.platform === "win32";
        };
        this.isLinux = function () {
            return process.platform === "linux";
        };
        this.isOSX = function () {
            return process.platform === "darwin";
        };

        this.getOS = function() {
            if (this.isWindows() === true)
                return "windows";
            else if (this.isLinux() === true)
                return "linux";
            else if (this.isOSX() === true)
                return "osx";

            return "unknown";
        };

        /**
         * @returns {boolean} 
         */
        this.isIPPInstalled = function () {
            if (this.isWindows() === true)
            {
                var baseDir = "C:/Program Files%ARCH%/IntelSWTools/";
                baseDir = baseDir.replace("%ARCH%", this.arch === "x64" ? " (x86)" : "");

                if (fs.existsSync(baseDir))
                    return true;
            }
            else
            {
                
            }

            return false;
        };

        this.getIPPPath = function () {
            if (this.isIPPInstalled() === false)
                return null;

            if (this.isWindows() === true) {
                var baseDir = "C:/Program Files%ARCH%/IntelSWTools/";
                baseDir = baseDir.replace("%ARCH%", this.arch === "x64" ? " (x86)" : "");

                var directories = fs.readdirSync(baseDir);
                directories = directories.filter(function (dir) {
                    return dir.indexOf("compilers_and_libraries") >= 0;
                });
                directories.sort(function(a, b) {
                    return a < b ? 1 : -1;
                });

                var libraryPath = baseDir + directories[0] + "/windows/ipp/lib/" + (this.arch === "x64" ? "intel64_win" : "ia32_win");
                var libraries = fs.readdirSync(libraryPath);
                libraries = libraries.filter(function (filename) {
                    return filename.indexOf("mt.lib") >= 0;
                });

                for (var i = 0; i < libraries.length; ++i)
                    libraries[i] = libraryPath + "/" + libraries[i];

                return {
                    include: [
                        baseDir + directories[0] + "/windows/ipp/include"
                    ],
                    lib: libraries
                }
            }

            return null;
        }

        const self = this;
        function internalExecuteCommand(arr) {
            var results = [];
            var targetDirectory = "/";
            for (var i = 0; i < arr.length; ++i)
            {
                var tokens = arr[i].split(" ");
                tokens.filter(function (e) {
                    return e.length === 0;
                });
                var target = tokens.shift();

                if (target === "cd") {
                    targetDirectory += tokens[0];
                } else {
                    var ret = cmd(target, tokens, {
                        cwd: __dirname + "/" + targetDirectory
                    });

                    if (ret.status !== 0 && ret.status !== null)
                        return false;

                    results.push(ret);
                }
            }

            return true;
        }

        this.executeCommands = function (obj) {
            if (this.isWindows() === true && obj.hasOwnProperty("windows") === true) {
                return internalExecuteCommand(obj.windows);
            } else if (this.isLinux() === true && obj.hasOwnProperty("linux") === true) {
                return internalExecuteCommand(obj.linux);
            } else if (this.isOSX() === true && obj.hasOwnProperty("osx") === true) {
                return internalExecuteCommand(obj.osx);
            }

            return null;
        };

        this.isDependencyInstalled = function (dependencyName) {
            return fs.existsSync(`./deps/${dependencyName}`);
        }
    }

    return new BuildTool();
});