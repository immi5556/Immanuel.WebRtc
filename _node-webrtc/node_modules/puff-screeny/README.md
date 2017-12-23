![logo](https://raw.githubusercontent.com/async3619/puff/master/logo.png)

[![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/oqcjdk1670a9ydjg?svg=true)](https://ci.appveyor.com/project/async3619/puff-screeny)
[![Build Dependencies](https://david-dm.org/async3619/puff/status.svg)](https://david-dm.org/async3619/puff)
[![NPM published version](https://img.shields.io/npm/v/puff-screeny.svg)](https://www.npmjs.com/package/puff-screeny)
[![Project license](https://img.shields.io/npm/l/puff-screeny.svg)]()

# Puff

Nodejs Cross-platform screen sharing addon with incredible performance. Written with C++.


This project still in development and only supports on Node.js or Electron based environment only.
I'll extend supporting environment for Web based environment!

## Installation

```bash
git clone https://github.com/async3619/Puff
cd ./Puff
npm install
```
or

```bash
npm install puff-screeny
```

## Usage

```js
var puff = require("puff");

// Call getAvailableWindows() to retrive captureable windows
// you can use getAvailableScreens if you want to capture Screen instead of Window.
var captureTargets = puff.getAvailableWindows();
```

## Building

```bash
npm install # <-- if you haven't installed dependencies
npm run -- node-gyp build # <-- It just build addon only. not effects on dependencies.
```

##### Dependencies

This project needs ```CMake``` to build dependencies before build Puff module.

## Supports

| Platform / OS            | Windows | Linux | OS X |
|--------------------------|:-------:|:-----:|:----:|
| Electron                 |    O    |   X   |   X  |
| Web Browser (Emscripten) |    X    |   X   |   X  |

Puff currently supports Electron and Windows environment only.

See Also: [Support Table Information](https://github.com/async3619/Puff/blob/master/SUPPORT.md).

## License
<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

The project is licensed under the [MIT License](http://opensource.org/licenses/MIT).

Copyright © 2017 [TerNer (async3619@naver.com)](https://terner.me)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
