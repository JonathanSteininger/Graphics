<h1>Goal</h1>
<p>
    This is a fun project where I learn how to interact with the OpenGL interface and access the GPU to render applications.
</p>
<p>
    This is my first project that really uses c++ so I will definetly learn alot and be slow. Ill have alot of fun!
</p>

<h1>Specifications</h1>
<h2>Device</h2>
<p>
    I am running this on my macbook from 2017.
    using an intel based processor with ATI graphics.
    <ul>
        <li>Processor: 2.5 Ghz Quad-Core Intel Core i7</li>
        <li>Graphics: AMD Radeon R9 M370X 2GB</li>
    </ul>
</p>

<h2>OpenGL</h2>

<p>
    Due to apple not updating openGL drivers, </br>
    I am running on openGL 4.0 but any newer version should work.

    the shaders I have made use GLS version 1.20 due to apple not updating any drivers.

    <ul>
        <li>
            OpenGL: 4.0.
</p>

<h1>Running Application<h1>

I am using CMake to help compile and manage dependencies.

run cmake in the console with this command: "cmake -S . -B ./build/"

this will generate the files necessary for cmake to build the project. you might have to change the c++ compiler path in order to make it compile. You can do this by changing the the cxx copiler path in the CMakeLists.txt file to the full path of your compiler.

<h3>Compiling the project</h3>
now that you have generated the files needed to compile it. you can now compile it by running the following command: "cmake --build ./build/"

this will use the files that you generated eariler to compile the project. now you can run the outputApp.app file generated inside of ./build/.
