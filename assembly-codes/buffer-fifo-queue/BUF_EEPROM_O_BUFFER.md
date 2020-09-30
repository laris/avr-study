# O Buffer
* https://community.atmel.com/projects/o-buffer
* Posted by eanwar on Sun. Feb 28, 2010 - 01:53 PM
* TYPE: Complete code
* COMPILER/ASSEMBLER: CodeVisionAVR-Standard

AVR101

Having a system that regularly writes parameters to
the EEPROM can wear out the EEPROM, since it is only
guaranteed to endure 100 k erase/write cycles. Writing
the parameters to a circular buffer in EEPROM where
each of the elements in the buffer can endure 100 k
erase/write cycles can circumvent this. However, if
the system is exposed to RESET conditions, such as power
failures, the system needs to be able to identify the
correct position in the circular buffer again

see all my projects at
http://www.ehab.1free.ws/index.html

* ATTACHMENT(S): [O Buffer.zip](https://community.atmel.com/sites/default/files/project_files/O%20Buffer.zip)
* Tags: [Complete code](https://community.atmel.com/projects-types/complete-code) , [CodeVisionAVR-Standard](https://community.atmel.com/compilers/codevisionavr-standard)
