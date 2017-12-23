var fs = require("fs");

var buildTool = require("./build-tool.js");
var requiredDependencies = {
    "opencv-prebuilt": {
        commands: {
            windows: [
                "git clone https://github.com/async3619/opencv-prebuilt deps/opencv-prebuilt"
            ],
            linux: [],
            osx: []
        },
        lib: {
            windows: [
                `<(module_root_dir)/deps/opencv-prebuilt/lib/${buildTool.getOS()}/x64/opencv_core2413.lib`,
                `<(module_root_dir)/deps/opencv-prebuilt/lib/${buildTool.getOS()}/x64/opencv_imgproc2413.lib`,
                `<(module_root_dir)/deps/opencv-prebuilt/lib/${buildTool.getOS()}/x64/zlib.lib`
            ],
            linux: [],
            osx: []
        },
        include: {
            windows: [
                "<(module_root_dir)/deps/opencv-prebuilt/include"
            ],
            linux: [],
            osx: []
        }
    }
};
var dependencies = {
};

process.stdout.write("Check out all of dependency installed ... ");

var notInstalledDependencies = {};
for (var dependencyName in requiredDependencies) {
    if (requiredDependencies.hasOwnProperty(dependencyName)) {
        if (buildTool.isDependencyInstalled(dependencyName) === false)
            notInstalledDependencies[dependencyName] = requiredDependencies[dependencyName];
        else
            requiredDependencies[dependencyName].is_installed = true;
    }
}

if (Object.keys(notInstalledDependencies).length > 0) {
    process.stdout.write("NOT INSTALLED\n");

    for (var dependencyName in notInstalledDependencies) {
        if (notInstalledDependencies.hasOwnProperty(dependencyName) === true) {
            process.stdout.write("Try to clone and build dependency '" + dependencyName + "' ... ");

            var ret = buildTool.executeCommands(notInstalledDependencies[dependencyName].commands, `deps/${dependencyName}`);
            if (ret === false) {
                process.stdout.write("FAILED\n");
                return;
            } else {
                process.stdout.write("SUCCEEDED\n");
                requiredDependencies[dependencyName].is_installed = true;
            }
        }
    }
} else {
    process.stdout.write("INSTALLED\n");
}

process.stdout.write("Check out Intel IPP can be used ... ");

// Check IPP can be used
if (buildTool.isIPPInstalled() === true) {
    process.stdout.write("YES\n");
    dependencies.IPP = buildTool.getIPPPath();
} else {
    process.stdout.write("NO\n");
}

console.log("\nDone!\n");
console.log("Print out generated build tool file ...");

var jsonOutput = {
    include_dirs: [],
    libraries: [],
    defines: []
};

if (buildTool.isIPPInstalled() === true) {
    jsonOutput.defines.push("_IPP_");
}

for (var key in dependencies) {
    if (dependencies.hasOwnProperty(key) === true)
    {
        if (dependencies[key].hasOwnProperty("include") === true) {
            for (var i = 0; i < dependencies[key].include.length; ++i) {
                jsonOutput.include_dirs.push(dependencies[key].include[i]);
            }
        }

        if (dependencies[key].hasOwnProperty("lib") === true) {
            for (var i = 0; i < dependencies[key].lib.length; ++i) {
                jsonOutput.libraries.push(dependencies[key].lib[i]);
            }
        }
    }
}

for (var key in requiredDependencies) {
    if (requiredDependencies.hasOwnProperty(key) === true) {
        if (requiredDependencies[key].hasOwnProperty("include") === true) {
            var includes = requiredDependencies[key].include[buildTool.getOS()];
            for (var i = 0; i < includes.length; ++i) {
                jsonOutput.include_dirs.push(includes[i]);
            }
        }

        if (requiredDependencies[key].hasOwnProperty("lib") === true)
        {
            var libraries = requiredDependencies[key].lib[buildTool.getOS()];
            for (var i = 0; i < libraries.length; ++i) {
                jsonOutput.libraries.push(libraries[i]);
            }
        }
    }

    if (requiredDependencies[key].hasOwnProperty("is_installed") === true)
        if (requiredDependencies[key].is_installed === true)
            jsonOutput.defines.push(`_${key.toUpperCase().replace("-prebuilt", "").replace("-", "_")}_`);
}

fs.writeFileSync("build-tool.json", JSON.stringify(jsonOutput, null, 4));
console.log(JSON.stringify(jsonOutput, null, 4));