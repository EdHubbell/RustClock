# RustClock

This project contains the code to run a Rust Clock. What is that? Well, it was a clock designed by a guy with the last name of Rust. Then it got old and didn't work anymore, so I stuck an Arduino in it to fix it. And pulled out a shit-ton of old wires and transformers and whatnot. So it lost a little weight.

The clock itself is a work of art - Solid brass frame that is well machined and mitre cut, black felt background on the lights. The electronics that were in there were beyond what I was prepared to debug. By prepared, I mean intellectually. I had no idea what the hell all that stuff was. What I could see was that someone worked really hard on it, and that this thing needed to survive and run and be treasured. 

The clock has 
- 12 red lights (hour)
- 5 amber lights (10 minutes) 
- 4 yellow lights in line with one green light (minutes, with the green light the '+5' increment)

They aren't LED lights, no sir. Just regular old lights. 

The only code here is the required file to run on an Arduino Mega. I used a Mega because it has plenty of digital outputs to do all the things that a clock needs to do. 

I also added a real time clock module - http://tronixstuff.com/2014/12/01/tutorial-using-ds1307-and-ds3231-real-time-clock-modules-with-arduino/

Other than that, a power supply and a bunch of resistors and transistors. It seems to work. Hopefully it will run for generations to come. Hopefully github will still be around in case someone needs to get to the code. 

~Ed

