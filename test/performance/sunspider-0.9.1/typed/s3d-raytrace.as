/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */
package {

function createVector(x:int,y:int,z:int):Array {
    return new Array(x,y,z);
}

function sqrLengthVector(self:Array):Number {
    return self[0] * self[0] + self[1] * self[1] + self[2] * self[2];
}

function lengthVector(self:Array):Number {
    return Math.sqrt(self[0] * self[0] + self[1] * self[1] + self[2] * self[2]);
}

function addVector(self:Array, v:Array):Array {
    self[0] += v[0];
    self[1] += v[1];
    self[2] += v[2];
    return self;
}

function subVector(self:Array, v:Array):Array {
    self[0] -= v[0];
    self[1] -= v[1];
    self[2] -= v[2];
    return self;
}

function scaleVector(self:Array, scale:Number):Array {
    self[0] *= scale;
    self[1] *= scale;
    self[2] *= scale;
    return self;
}

function normaliseVector(self:Array):Array {
    var len:Number = Math.sqrt(self[0] * self[0] + self[1] * self[1] + self[2] * self[2]);
    self[0] /= len;
    self[1] /= len;
    self[2] /= len;
    return self;
}

function add(v1:Array, v2:Array):Array {
    return new Array(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]);
}

function sub(v1:Array, v2:Array):Array {
    return new Array(v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]);
}

function scalev(v1:Array, v2:Array):Array {
    return new Array(v1[0] * v2[0], v1[1] * v2[1], v1[2] * v2[2]);
}

function dot(v1:Array, v2:Array):Number {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

function scale(v:Array, scale:Number):Array {
    return [v[0] * scale, v[1] * scale, v[2] * scale];
}

function cross(v1:Array, v2:Array):Array {
    return [v1[1] * v2[2] - v1[2] * v2[1], 
            v1[2] * v2[0] - v1[0] * v2[2],
            v1[0] * v2[1] - v1[1] * v2[0]];

}

function normalise(v:Array):Array {
    var len:Number = lengthVector(v);
    return [v[0] / len, v[1] / len, v[2] / len];
}

function transformMatrix(self:Array, v:Array):Array {
    var vals = self;
    var x  = vals[0] * v[0] + vals[1] * v[1] + vals[2] * v[2] + vals[3];
    var y  = vals[4] * v[0] + vals[5] * v[1] + vals[6] * v[2] + vals[7];
    var z  = vals[8] * v[0] + vals[9] * v[1] + vals[10] * v[2] + vals[11];
    return [x, y, z];
}

function invertMatrix(self:Array):Array {
    var temp:Array = new Array(16);
    var tx:Number = -self[3];
    var ty:Number = -self[7];
    var tz:Number = -self[11];
    for (var h:uint = 0; h < 3; h++)
        for (var v:uint = 0; v < 3; v++)
            temp[h + v * 4] = self[v + h * 4];
    for (var i:uint = 0; i < 11; i++)
        self[i] = temp[i];
    self[3] = tx * self[0] + ty * self[1] + tz * self[2];
    self[7] = tx * self[4] + ty * self[5] + tz * self[6];
    self[11] = tx * self[8] + ty * self[9] + tz * self[10];
    return self;
}


// Triangle intersection using barycentric coord method
public class Triangle {
    var normal:Array;
    var nu:Number;
    var nv:Number;
    var nd:Number;
    var eu:uint;
    var ev:uint;
    var nu1:Number;
    var nv1:Number;
    var nu2:Number;
    var nv2:Number;
    var material:Array;
    var axis:uint;
    var shader:Function;
public function Triangle(p1:Array, p2:Array, p3:Array):void {
    var edge1:Array = sub(p3, p1);
    var edge2:Array = sub(p2, p1);
    var normal:Array = cross(edge1, edge2);
    if (Math.abs(normal[0]) > Math.abs(normal[1]))
        if (Math.abs(normal[0]) > Math.abs(normal[2]))
            this.axis = 0; 
        else 
            this.axis = 2;
    else
        if (Math.abs(normal[1]) > Math.abs(normal[2])) 
            this.axis = 1;
        else 
            this.axis = 2;
    var u:int = (this.axis + 1) % 3;
    var v:int = (this.axis + 2) % 3;
    var u1:int = edge1[u];
    var v1:int = edge1[v];
    
    var u2:int = edge2[u];
    var v2:int = edge2[v];
    this.normal = normalise(normal);
    this.nu = normal[u] / normal[this.axis];
    this.nv = normal[v] / normal[this.axis];
    this.nd = dot(normal, p1) / normal[this.axis];
    var det:int = u1 * v2 - v1 * u2;
    this.eu = p1[u];
    this.ev = p1[v]; 
    this.nu1 = u1 / det;
    this.nv1 = -v1 / det;
    this.nu2 = v2 / det;
    this.nv2 = -u2 / det; 
    this.material = [0.7, 0.7, 0.7];
}

public function intersect(orig:Array, dir:Array, near:Number, far:Number):* {   // returntype is * due to returning of nulls
    var u:uint = (this.axis + 1) % 3;
    var v:uint = (this.axis + 2) % 3;
    var d:uint = dir[this.axis] + this.nu * dir[u] + this.nv * dir[v];
    var t:Number = (this.nd - orig[this.axis] - this.nu * orig[u] - this.nv * orig[v]) / d;
    if (t < near || t > far)
        return null;
    var Pu:uint = orig[u] + t * dir[u] - this.eu;
    var Pv:uint = orig[v] + t * dir[v] - this.ev;
    var a2:uint = Pv * this.nu1 + Pu * this.nv1;
    if (a2 < 0) 
        return null;
    var a3:uint = Pu * this.nu2 + Pv * this.nv2;
    if (a3 < 0) 
        return null;

    if ((a2 + a3) > 1) 
        return null;
    return t;
}
} // class Triangle

public class Scene {
    var zero:Array = new Array(0,0,0);
    var triangles:Array;
    var lights:Array;
    var ambient:Array;
    var background:Array;
public function Scene(a_triangles):void {
    this.triangles = a_triangles;
    this.lights = [];
    this.ambient = [0,0,0];
    this.background = [0.8,0.8,1];
}


public function intersect(origin:Array, dir:Array, near:Number = 0, far:Number = 0):Array {
    var closest:Triangle = null;
    for (var i:uint = 0; i < this.triangles.length; i++) {
        var triangle:Triangle = this.triangles[i];
        var d:* = triangle.intersect(origin, dir, near, far);
        if (d == null || d > far || d < near)
            continue;
        far = d;
        closest = triangle;
    }
    
    if (!closest)
        return [this.background[0],this.background[1],this.background[2]];
        
    var normal:Array = closest.normal;
    var hit:Array = add(origin, scale(dir, far));
    if (dot(dir, normal) > 0)
        normal = [-normal[0], -normal[1], -normal[2]];
    
    var colour:Array = null;
    if (closest.shader) {
        colour = closest.shader(closest, hit, dir);
    } else {
        colour = closest.material;
    }
    
    // do reflection
    var reflected:Array = null;
    if (colour.reflection > 0.001) {
        var reflection:Array = addVector(scale(normal, -2*dot(dir, normal)), dir);
        reflected = this.intersect(hit, reflection, 0.0001, 1000000);
        if (colour.reflection >= 0.999999)
            return reflected;
    }
    
    var l:Array = [this.ambient[0], this.ambient[1], this.ambient[2]];
    for (var i:uint = 0; i < this.lights.length; i++) {
        var light:Array = this.lights[i];
        var toLight:Array = sub(light, hit);
        var distance:Number = lengthVector(toLight);
        scaleVector(toLight, 1.0/distance);
        distance -= 0.0001;
        if (this.blocked(hit, toLight, distance))
            continue;
        var nl:Number = dot(normal, toLight);
        if (nl > 0)
            addVector(l, scale(light.colour, nl));
    }
    l = scalev(l, colour);
    if (reflected) {
        l = addVector(scaleVector(l, 1 - colour.reflection), scaleVector(reflected, colour.reflection));
    }
    return l;
}

public function blocked(O:Array, D:Array, far:Number):Boolean {
    var near:Number = 0.0001;
    var closest:Array = null;
    for (var i:uint = 0; i < this.triangles.length; i++) {
        var triangle = this.triangles[i];   
        var d:* = triangle.intersect(O, D, near, far);
        if (d == null || d > far || d < near)
            continue;
        return true;
    }
    
    return false;
}
} // class Scene

// this camera code is from notes i made ages ago, it is from *somewhere* -- i cannot remember where
// that somewhere is
public class Camera {
    var directions:Array;
    var origin:Array;
public function Camera(origin:Array, lookat:Array, up:Array):void {
    var zaxis = normaliseVector(subVector(lookat, origin));
    var xaxis = normaliseVector(cross(up, zaxis));
    var yaxis = normaliseVector(cross(xaxis, subVector([0,0,0], zaxis)));
    var m = new Array(16);
    m[0] = xaxis[0]; m[1] = xaxis[1]; m[2] = xaxis[2];
    m[4] = yaxis[0]; m[5] = yaxis[1]; m[6] = yaxis[2];
    m[8] = zaxis[0]; m[9] = zaxis[1]; m[10] = zaxis[2];
    invertMatrix(m);
    m[3] = 0; m[7] = 0; m[11] = 0;
    this.origin = origin;
    this.directions = new Array(4);
    this.directions[0] = normalise([-0.7,  0.7, 1]);
    this.directions[1] = normalise([ 0.7,  0.7, 1]);
    this.directions[2] = normalise([ 0.7, -0.7, 1]);
    this.directions[3] = normalise([-0.7, -0.7, 1]);
    this.directions[0] = transformMatrix(m, this.directions[0]);
    this.directions[1] = transformMatrix(m, this.directions[1]);
    this.directions[2] = transformMatrix(m, this.directions[2]);
    this.directions[3] = transformMatrix(m, this.directions[3]);
}

public function generateRayPair(y:Number):Array {
    var rays:Array = new Array(new Object(), new Object());
    rays[0].origin = this.origin;
    rays[1].origin = this.origin;
    rays[0].dir = addVector(scale(this.directions[0], y), scale(this.directions[3], 1 - y));
    rays[1].dir = addVector(scale(this.directions[1], y), scale(this.directions[2], 1 - y));
    return rays;
}

public function render(scene:Scene, pixels:Array, width:uint, height:uint):void {
    var cam:Camera = this;
    var row:int = 0;
    renderRows(cam, scene, pixels, width, height, 0, height);
}
} // class Camera

function renderRows(camera:Camera, scene:Scene, pixels:Array, width:uint, height:uint, starty:uint, stopy:uint):void {
    for (var y:uint = starty; y < stopy; y++) {
        var rays:Array = camera.generateRayPair(y / height);
        for (var x:uint = 0; x < width; x++) {
            var xp:Number = x / width;
            var origin:Array = addVector(scale(rays[0].origin, xp), scale(rays[1].origin, 1 - xp));
            var dir:Array = normaliseVector(addVector(scale(rays[0].dir, xp), scale(rays[1].dir, 1 - xp)));
            var l:Array = scene.intersect(origin, dir);
            pixels[y][x] = l;
        }
    }
}





function raytraceScene():Array
{
    var startDate:uint = new Date().getTime();
    var numTriangles:uint = 2 * 6;
    var triangles:Array = new Array();//numTriangles);
    var tfl:Array = createVector(-10,  10, -10);
    var tfr:Array = createVector( 10,  10, -10);
    var tbl:Array = createVector(-10,  10,  10);
    var tbr:Array = createVector( 10,  10,  10);
    var bfl:Array = createVector(-10, -10, -10);
    var bfr:Array = createVector( 10, -10, -10);
    var bbl:Array = createVector(-10, -10,  10);
    var bbr:Array = createVector( 10, -10,  10);
    
    // cube!!!
    // front
    var i:uint = 0;
    
    triangles[i++] = new Triangle(tfl, tfr, bfr);
    triangles[i++] = new Triangle(tfl, bfr, bfl);
    // back
    triangles[i++] = new Triangle(tbl, tbr, bbr);
    triangles[i++] = new Triangle(tbl, bbr, bbl);
    //        triangles[i-1].material = [0.7,0.2,0.2];
    //            triangles[i-1].material.reflection = 0.8;
    // left
    triangles[i++] = new Triangle(tbl, tfl, bbl);
    //            triangles[i-1].reflection = 0.6;
    triangles[i++] = new Triangle(tfl, bfl, bbl);
    //            triangles[i-1].reflection = 0.6;
    // right
    triangles[i++] = new Triangle(tbr, tfr, bbr);
    triangles[i++] = new Triangle(tfr, bfr, bbr);
    // top
    triangles[i++] = new Triangle(tbl, tbr, tfr);
    triangles[i++] = new Triangle(tbl, tfr, tfl);
    // bottom
    triangles[i++] = new Triangle(bbl, bbr, bfr);
    triangles[i++] = new Triangle(bbl, bfr, bfl);
    
    //Floor!!!!
    var green:Array = createVector(0.0, 0.4, 0.0);
    var grey:Array = createVector(0.4, 0.4, 0.4);
    grey.reflection = 1.0;
    var floorShader:Function = function(tri:Triangle, pos:Array, view:Array) {
        var x:Number = ((pos[0]/32) % 2 + 2) % 2;
        var z:Number = ((pos[2]/32 + 0.3) % 2 + 2) % 2;
        if (x < 1 != z < 1) {
            //in the real world we use the fresnel term...
            //    var angle = 1-dot(view, tri.normal);
            //   angle *= angle;
            //  angle *= angle;
            // angle *= angle;
            //grey.reflection = angle;
            return grey;
        } else 
            return green;
    }
    var ffl:Array = createVector(-1000, -30, -1000);
    var ffr:Array = createVector( 1000, -30, -1000);
    var fbl:Array = createVector(-1000, -30,  1000);
    var fbr:Array = createVector( 1000, -30,  1000);
    triangles[i++] = new Triangle(fbl, fbr, ffr);
    triangles[i-1].shader = floorShader;
    triangles[i++] = new Triangle(fbl, ffr, ffl);
    triangles[i-1].shader = floorShader;
    
    var _scene:Scene = new Scene(triangles);
    _scene.lights[0] = createVector(20, 38, -22);
    _scene.lights[0].colour = createVector(0.7, 0.3, 0.3);
    _scene.lights[1] = createVector(-23, 40, 17);
    _scene.lights[1].colour = createVector(0.7, 0.3, 0.3);
    _scene.lights[2] = createVector(23, 20, 17);
    _scene.lights[2].colour = createVector(0.7, 0.7, 0.7);
    _scene.ambient = createVector(0.1, 0.1, 0.1);
    //  _scene.background = createVector(0.7, 0.7, 1.0);
    
    var size:uint = 30;
    var pixels:Array = new Array();
    for (var y:uint = 0; y < size; y++) {
        pixels[y] = new Array();
        for (var x = 0; x < size; x++) {
            pixels[y][x] = 0;
        }
    }

    var _camera:Camera = new Camera(createVector(-40, 40, 40), createVector(0, 0, 0), createVector(0, 1, 0));
    _camera.render(_scene, pixels, size, size);

    return pixels;
}

function arrayToCanvasCommands(pixels:Array):String
{
    var s = '<canvas id="renderCanvas" width="30px" height="30px"></canvas><scr' + 'ipt>\nvar pixels = [';
    var size = 30;
    for (var y = 0; y < size; y++) {
        s += "[";
        for (var x = 0; x < size; x++) {
            s += "[" + pixels[y][x] + "],";
        }
        s+= "],";
    }
    s += '];\n    var canvas = document.getElementById("renderCanvas").getContext("2d");\n\
\n\
\n\
    var size = 30;\n\
    canvas.fillStyle = "red";\n\
    canvas.fillRect(0, 0, size, size);\n\
    canvas.scale(1, -1);\n\
    canvas.translate(0, -size);\n\
\n\
    if (!canvas.setFillColor)\n\
        canvas.setFillColor = function(r, g, b, a) {\n\
            this.fillStyle = "rgb("+[Math.floor(r * 255), Math.floor(g * 255), Math.floor(b * 255)]+")";\n\
    }\n\
\n\
for (var y = 0; y < size; y++) {\n\
  for (var x = 0; x < size; x++) {\n\
    var l = pixels[y][x];\n\
    canvas.setFillColor(l[0], l[1], l[2], 1);\n\
    canvas.fillRect(x, y, 1, 1);\n\
  }\n\
}</scr' + 'ipt>';

    return s;
}

// main entry point for running testcase
function runTest():void{
var testOutput:String = arrayToCanvasCommands(raytraceScene());
} //runTest()

// warm up run of testcase
runTest();
var startTime:uint = new Date().getTime();
runTest();
var time:uint = new Date().getTime() - startTime;
print("metric time " + time);

} // package