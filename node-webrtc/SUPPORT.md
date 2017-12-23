## Supporting Table

You can use several features of Puff with several platform. the table on right below shows features that currently supports. you can get more information if click name of feature.

| Features / OS                | Windows | Linux | OS X | Note                                         |
|------------------------------|:-------:|:-----:|:----:|----------------------------------------------|
| [Screensharing](#screensharing)|    O    |   X   |   X  | use Intel® IPP, AMD® Framewave as default    |
| [HW-Accelerated Screensharing](#hardware-accelerated-screensharing) |    O    |   X   |   X  | same with above, use GPU-Accelerating if can |

<br/>

### Screensharing 

Default setting of screensharing limited as use CPU-resource only. but this feature will use some CPU-Accelerated library to get incredible performance. such as:

* [Intel® Integrated Performance Primitives](https://software.intel.com/en-us/intel-ipp)

build-tool automatically use OpenCV as default if these libraries are not installed. (slower then Intel® IPP)

##### Example

```js
var puff = require('puff');
var screens = puff.getAvailableScreens();
var width = /* random width value */;
var height = /* random height value */;
var data = /* Image raw data container */;
var fps = 30;

screens[0].startCapture(data, width, height, 30, function () {
    /* do post-processing or everything */
});
```

:warning: _Caution: Those library can be used when compile it if installed on computer, build-tool of Puff will make sure if library installed on computer._

| Windows | Linux | OS X |
|:-------:|:-----:|:----:|
|    O    |   X   |   X  |

<br/>

### Hardware-Accelerated Screensharing

You can use GPU resource to increase more performance than using CPU resource only. This feature will use these library to make sure using GPU resource:

* [DirectX 11](http://www.geforce.com/hardware/technology/dx11) (Windows only) - _which means you can compile / run it on Windows 8.1 and DX11 supported graphics cards at least._

##### Example

```js
var puff = require('puff');
var screens = puff.getAvailableScreens();
var width = /* random width value */;
var height = /* random height value */;
var data = /* Image raw data container */;
var fps = 30;

screens[0].startCapture(data, width, height, 30, function () {
    /* do post-processing or everything */
}); // this function will use HW-Accelerating if can.
```
| Windows | Linux | OS X |
|:-------:|:-----:|:----:|
|    O    |   X   |   X  |

<br/>