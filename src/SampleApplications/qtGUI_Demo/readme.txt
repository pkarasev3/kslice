README:

I'm still trying to get remote desktop working, until then I'm going to just have to ssh where I'm at over to the zoe server for easy access and work.

Good sites:
http://www.qtcentre.org/forum.php?s=126099a886084fad7d6368cfccc05ad8
http://www.digitalfanatics.org/projects/qt_tutorial/chapter14.html
http://sector.ynet.sk/qt4-tutorial/

---

Step 1: Understanding how widgets work, how they're linked to code, taking input and displaying output through QT windows.

Questions: How did you build the ACL Model? Probably not QT Designer or Creator. All the tutorials I'm seeing have a .pro file along with the header, class and main files so that a compiler can parse the ui into code that is usable by the machine. Did you guys make that pro file or find another way inside of vs studio. QMake? Make?

Answer:  Hey Marin they build the ACLmodel using CMake. This allows you to link all the libraries you need together and not care about the details. When you build a project with Cmake it asks you to input where the qmake executable is and therefore takes care of this problem for you. I would recommend you build your gui with CMake and include the VTK, and Qt libraries and link them using CMake. I will upload a CMakeList.txt to show you how to do this. Good Luck and if you have questions txt me. Matias Perez

Different widgets that aren't located in QT Designer or Creater, once again leading back to the question of how the ACL program was done. Did you guys open up and change the ui file to implement? Where did you get the widgets, how does their downloading / implementing look like.

Step 2: Either continueing the way I'm going with linux to the very end, or fixing windows make and qt if it comes down to that. If it turns out Pete and Ivan didn't use qmake and then make, then perhaps using VSstudio and trying to use windows down that path.

Step 3: Implementing load image code. Browse code is simple enough. Right now images are just placeholder and don't load. For code loading we're going to set whatever the image widget is going to be called ex. image1->setPath(file://blahblah). First I need to figure out how to add widgets that aren't there yet, or rename placeholders.

Step 4: Onto rev 1.