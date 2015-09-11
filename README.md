#AMExtractor#

AMExtractor = Android Memory Extractor

Memory forensic tools provide a thorough way to detect malwares and ferret out cyber crimes. [LiME](https://github.com/504ensicsLabs/LiME) is very popular among community. But LiME and similar LKM based tools cannot be used without target source code or LKM support. 
As far as I know, stock ROM of Nexus series doesn't support LKM at all; Samsung Galaxy series has a sanity check and won't allow unofficial modules.
Rather than LKM, AMExtractor use /dev/kmem to run code in kernel space. AMExtractor is tested on several devices shipped with different versions of Android, including Galaxy Nexus, Nexus 4, Nexus 5, Samsung Galaxy S4.

#HOW TO USE#

Define target phone's configuration in config.h. Now there are preset config of Galaxy Nexus, Nexus4, Nexus 5, Samsung Galaxy S4(I9500).

Linux has three different memory model: FLAT_MEM, SPARSE_MEM and DISCONTIG_MEM. Different memory model
use different method to translate Page Frame Number to Page Pointer. Define one according to disassemble result of vm_normal_page.

You also need know sizeof(struct page). Define it as STRUCT_PAGE_SIZE. Typical size is 32.

Define trigger method. Usually fsync method of /dev/ptmx will do the trick.

