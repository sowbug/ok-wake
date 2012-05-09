OK Wake
=======

[OK Wake](http://www.sowbug.com/ok-wake) is a device that lets pre-literate kids know it's OK to get out of bed.

Copyright © 2012 Mike Tsao.

This project started with a lunchtime conversation with a friend whose kids are too young to read, but not too young to get out of bed at the crack of dawn and wake up their parents. We wondered whether a little blinking light would be a satisfactory signal to keep the kids in bed until the designated hour.

Thus was born the OK Wake. It divides the day into three sections: night, twilight, and dawn. Most of the 24-hour cycle is night, and the device does nothing. If wake-up time is 6am, then twilight would start at 5:30am with a slow red blink, gradually increasing in frequency until dawn at 6am, when the blink changes to a green “breathing” blink. Dawn continues for 10 minutes, and then night resumes.

While working on the feature set, I had some very intricate ideas for dealing with Daylight Saving Time. Either I’d just handle it (meaning the kids would be confused by the abrupt shift twice a year), or I’d do a gradual shift in the week or so leading up to the time change. It got complicated, especially when I thought about the case where the family traveled to a different timezone. I finally decided I was overthinking the problem, and added a button. Pressing the button sets the alarm to 12 hours from the current time (e.g., press at 6pm to wake at 6am). I want to call this the antipode, as it’s the opposite point on a circular clock, but I think I’m misusing that term. I think the 6pm/6am solution is simple and elegant; as a parent I know kids want to wake up around 6am, and 6pm is right around the time parents are thinking they’d be totally OK with the kids going to bed right now.

The circuit is based on an ATtiny25 microcontroller and a Philips/NXP PCF8523 real-time clock chip. I chose the ‘8523 for two reasons: first, it is not the Maxim DS1307, which is shockingly expensive for a chip that’s basically a digital counter; and second, because it uses I2C, which I wanted to learn about.

Early in the design phase, I concluded that picking the ‘8523 eliminated my current favorite AVR, the ATtiny13a, because that chip didn’t support I2C. But as I later learned, the ATtiny25 doesn’t really support I2C either, except to the extent that it has a USART that can be tricked into assisting with I2C bit-banging. In the end I didn’t even use the USART anyway, instead relying on a fantastic small I2C library that uses two GPIO pins. But I’m happy I had the ‘25 rather than the ‘13a, because I used up about 1,800 of the 2KB program space, and wouldn’t have had much fun trying to squeeze that into the 1KB space of the ‘13a.

The firmware took one weekend day and two nights to write, which felt like a long time for a fairly simple set of features. I started out using an Arduino as the hardware platform, which was marginally useful for getting up and running, but eventually I threw out all that code when rewriting for the ‘25. The most frustrating part by far was dealing with the packed BCD (binary-coded decimal) that appears to be commonplace with RTC chips. I did just enough time math that I had to write bcd add/subtract routines (fortunately there are canonical bit-twiddling solutions on the web), and BCD-to-time-aware-decimal conversion routines. Unit tests helped a lot here.

