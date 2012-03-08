QSnapshot
=========

This is a screen capture program, inspired by `KSnapshot`_.

Features
--------

* Capture full screen
* Capture window under cursor
* Capture section of a window
* Capture selected region

Build dependency
----------------

You need following tools:

* `CMake`_
* `Qt`_
* C++0x supported toolchain
    * Microsoft Windows
        `Microsoft Visual C++`_ >= 2010
    * Mac OS X
        * `XCode`_
        * `GNU Compiler Collection`_ >= 4.6 [#]_
    * GNU/Linux
        * `GNU Compiler Collection`_ >= 4.6
        * `X11 shape extension`_ (optional)

How to build
------------

::

    mkdir build
    cd build
    cmake ..
    make install

.. [#] Since `XCode`_ does not provide this version, you could use `MacPorts`_
    or `Homebrew`_ to install one.

.. _CMake: http://www.cmake.org/
.. _GNU Compiler Collection: http://gcc.gnu.org/
.. _Homebrew: http://mxcl.github.com/homebrew/
.. _MacPorts: http://www.macports.org/
.. _Microsoft Visual C++: http://www.microsoft.com/visualstudio/en-us/products/2010-editions
.. _KSnapshot: http://kde.org/applications/graphics/ksnapshot/
.. _Qt: http://qt.nokia.com/products/
.. _X11 shape extension: http://www.x.org/releases/X11R7.6/doc/libXext/shapelib.html
.. _XCode: https://developer.apple.com/xcode/
