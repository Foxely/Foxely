## Foxely
Foxely is a powerful, dynamically typed written in C++ without any external dependencies (except for stdlib). It is a class-based scripting language inspired a little bit by JS.

## Features
 - singlepass compiler
 - repl
 - dynamic typing
 - classes and inheritance
 - higher order functions and classes
 - lexical scoping
 - nested classes
 - closures
 - garbage collection
 - operator overriding

## What Foxely code looks like
```js
Vector :: class {
	// constructor
	init (a, b, c) {
	    // instance variables
		this.x = a;
        this.y = b;
        this.z = c;
	}

	// instance method (built-in operator overriding)
	operator + (v) {
        return Vector(this.x + v.x, this.y + v.y, this.z + v.z);
	}

	string() {
		return "[" + str(this.x) + ", " + str(this.y) + ", " + str(this.z) + "]";
	}
}

// initialize a new vector object
v1 := Vector(1, 2, 3);

// initialize a new vector object
v2 := Vector(4, 5, 6);

// call + function in the vector object
v3 := v1 + v2;

// returns string "[1,2,3] + [4,5,6] = [5,7,9]"
print "% + % = %\n", v1.string(), v2.string(), v3.string();
```
## Installation
To compile the library, type `make` in the root folder.
To compile the interpreter, go to `examples` folder and type `make`.


## API Reference
No available right now.

## How to use?
To use the foxely interpreter:
  - make sure that your compile the code [here](#installation)
  - to run the interpreter, type this code `./foxely <file>.fox`
