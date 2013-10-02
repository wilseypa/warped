# How to contribute

Due to the nature of academic projects, contributors join and leave
frequently. To keep the code base usable for future contributors, follow these
guidelines when writing code.

## Getting Started

* Make sure you have a github account [GitHub account](https://github.com/signup/free).
* Read the materials in the Resources section of this document. 
* If applicable, submit an issue on GitHub describing the issue are you working on.

## Making Changes

* Create a topic branch from where you want your base to work.
	* You usually want to branch off of the master branch.
    * All work should be done in a topic branch: don't commit directly to
      master. Especially don't commit broken or incomplete features to master.
* Commit changes to your topic branch in logical units.
	* The first line of the commit message should be a descriptive summary of the commit. 
	* The first line should not be longer the 50 columns
    * If the commit is not small, follow the description by a single blank
      line and begin a description on the third line.
	* The description lines should not exceed 72 columns.
* Add tests for any code that you write. See the Testing section below for
  more details. Tests are extremely important to ensure that not only is you
  code correct, but more importantly that future code changes don't break your
  code.
* Once you've written tests for your code and they and all existing tests
  pass, you are ready to merge your branch.
    * If your changes do not interfere with any other contributors' work, then
      you can merge your branch into master.
    * If your changes do affect other contributors, open a pull request
      detailing your changes.  Once other contributors have signed off on your
      request, you can merge your branch into master. See the Scott Schacon
      article for details on this process.

## Testing

For historical reasons, unit tests for WARPED are written using two different
frameworks: the more traditional, verbose
[CppUnit](http://cpptest.sourceforge.net), and the more lightweight
[Catch](http://catch-test.net/). Instructions for building and running the
tests can be found in the `test` directory. The Ken Beck book listed in the
resources is recommended reading.

Whichever framework you choose to use, you must write thorough tests for any
code that you commit to the project. Each test should tests exactly one thing:
don't lump a bunch of tests into a single function. Additionally, keep tests
for each class in their own files.

#### Framework documentation
* [Catch](https://github.com/philsquared/Catch/blob/master/docs/reference-index.md) 
  (additional discussion [here](http://www.levelofindirection.com/journal/tag/catch))
* [CppUnit](http://cpptest.sourceforge.net/tutorial.html)

## Style Guide

As with most style guides, the most important rule is to remain consistent
with the surrounding code. If a file differs from acceptable style, it is more
important that your code follow the conventions within the file than to have
one function styled differently than the rest of the file. Having said that,
if a file does differ from the accepted style, fixing it is a good idea. Do
not commit style changes at the same time as functional changes! This makes it
very difficult to review your changes, and creates unnecessary merge conflicts
if other people are working on the same code.

The formatting style is enforced using [Artistic
Style](astyle.sourceforge.ne). There is a `.astylerc` file in the root of the
repository which specifies the style used in the project's code. There are
AStyle plugins for most popular editors, or you can run it from the command
line with the a command like the following:

    astyle --options=.astylerc /src/your-file.cpp

Non-formatting style follows the [Google C++ Style Guide](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml). 
There are some guidelines there that are specific to Google. Here are some of
the ways that WARPED style differs from the Google style:

* All C++11 features are allowed.
* Using pointers as output parameters is discouraged unless a pointer is specifically needed. 
* References are preferred to pointers, since references cannot be null.
* Raw pointers should be avoided when possible in favor of C++11 smart pointers.
* Do not include a license header at the top of each file. Do include a
  comment at the top of each file describing the files usage and purpose.

Most importantly, remember that you are not writing code for yourself to read.
You are writing code that dozens of contributors in the future will have to
read. Comment your code liberally. Especially important are usage comments:
every file, class, and function that you write needs to have a comment
describing its usage and purpose.

## Resources
* The [Git book](http://git-scm.com/book) is indispensable if you haven't used Git before. 

* [This article](http://scottchacon.com/2011/08/31/github-flow.html) by Scott 
  Chacon, an engineer at GitHub, is a very good description of how GitHub uses
  their own service, and is the model for how we organize our workflow.

* The [Google C++ Style Guide](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml). 
  Although some of the guidelines are specific to Google themselves, the
  majority are very useful.
* *Test-Driven Development: By Example* by Kent Beck. This is a very good 
  introduction to Unit Testing and test driven development. If you are a
  University of Cincinnati student, the UC Library has both [physical and
  electronic copies available](http://uclid.uc.edu/record=b4376056~S39).
