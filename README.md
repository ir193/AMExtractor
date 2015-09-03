#AMExtractor#

Memory forensic tools provide a thorough way to detect malwares and ferret out cyber crimes. [LiME](https://github.com/504ensicsLabs/LiME) is very popular among community. But LiME and similar LKM based tools cannot be used without target source code or LKM support. 
As far as I know, stock ROM of Nexus series doesn't support LKM at all; Samsung Galaxy series has a sanity check and won't allow unofficial modules.
Rather than LKM, AMExtractor use /dev/kmem to run code in kernel space. AMExtractor is tested on several devices shipped with different versions of Android, including Galaxy Nexus, Nexus 4, Nexus 5, Samsung Galaxy S4.

