#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pci/pci.h>

#define XBUS_LEN 32
#define XBUF_LEN 1024

#define PCI_FILL_OPTIONS	(PCI_FILL_PHYS_SLOT)

#define VENDOR_NVIDIA			 0x10de
#define VENDOR_AMD				 0x1002
#define VENDOR_INTEL		     0x8086
#define CLASS_DISPLAY_CONTROLLER 0x03

#define DRIVER_INTEL  "intel"
#define DRIVER_NVIDIA "nvidia"
#define DRIVER_AMD	  "amdgpu"

#define XCONF_PATH "./xorg.conf"

#define SECTION_END "EndSection\n\n"

#define SECTION_LAYOUT \ 
"Section \"ServerLayout\"\n"\
"    Identifier    \"Layout0\"\n"\
"    InputDevice   \"Keyboard0\" \"CoreKeyboard\"\n"\
"    InputDevice   \"Mouse0\"    \"CorePointer\"\n"

#define SECTION_MONITOR \
"Section \"Monitor\"\n"\
"    Identifier     \"Monitor%d\"\n"\
SECTION_END

#define SECTION_DEVICE \
"Section \"Device\"\n"\
"    Identifier     \"Device%d\"\n"\
"    Driver         \"%s\"\n"\
"    BusID          \"%s\"\n"\
SECTION_END

#define SECTION_SCREEN_NVIDIA \
"Section \"Screen\"\n"\
"    Identifier     \"Screen%d\"\n"\
"    Device         \"Device%d\"\n"\
"    Monitor        \"Monitor%d\"\n"\
"    Option         \"AllowEmptyInitialConfiguration\" \"True\"\n"\
"    Option         \"Coolbits\" \"31\"\n"\
"    DefaultDepth   24\n"\
"    SubSection     \"Display\"\n"\
"        Depth       24\n"\
"    EndSubSection\n"\
SECTION_END

#define SECTION_SCREEN \
"Section \"Screen\"\n"\
"    Identifier     \"Screen%d\"\n"\
"    Device         \"Device%d\"\n"\
"    Monitor        \"Monitor%d\"\n"\
"    DefaultDepth   24\n"\
"    SubSection     \"Display\"\n"\
"        Depth       24\n"\
"    EndSubSection\n"\
SECTION_END





int main() {
    
	struct pci_access* pciaccess;
	struct pci_dev* dev;
	FILE *fp;

	if ( (pciaccess = pci_alloc()) == NULL)
		perror("Could not init pci access");

	pci_init(pciaccess);
	pci_scan_bus(pciaccess);

		
	if ( (fp = fopen(XCONF_PATH, "w")) == NULL)
			perror("Could not open xconf file");
	
	int N = 0;
	char buf[XBUF_LEN] = {0};
	
	for (dev = pciaccess->devices; dev; dev = dev->next) {
		if ( (dev->device_class >> 8) != CLASS_DISPLAY_CONTROLLER)
			continue;
		
		if(pci_fill_info(dev, PCI_FILL_OPTIONS) == 0)
			perror("Could not fill pci info");		
		
		char pci[XBUS_LEN] = {0};
		snprintf(pci, XBUS_LEN, "PCI:%01x:%01x:%01x", dev->bus, dev->dev, dev->func);
		
		snprintf(buf, XBUF_LEN, SECTION_MONITOR, N);
		fputs(buf, fp);
		
		if(dev->vendor_id == VENDOR_AMD   ) {			
			memset(&buf, 0, XBUF_LEN);
			snprintf(buf, XBUF_LEN, SECTION_DEVICE, N, DRIVER_AMD, pci);
			fputs(buf, fp);	
			
			memset(&buf, 0, XBUF_LEN);
			snprintf(buf, XBUF_LEN, SECTION_SCREEN, N, N, N);
			fputs(buf, fp);
			
			N++;
		}
		
		if(dev->vendor_id == VENDOR_NVIDIA) {
			memset(&buf, 0, XBUF_LEN);
			snprintf(buf, XBUF_LEN, SECTION_DEVICE, N, DRIVER_NVIDIA, pci);
			fputs(buf, fp);	
			
			memset(&buf, 0, XBUF_LEN);
			snprintf(buf, XBUF_LEN, SECTION_SCREEN_NVIDIA, N, N, N);
			fputs(buf, fp);
			
			N++;
		}
		
		if(dev->vendor_id == VENDOR_INTEL ) {
			memset(&buf, 0, XBUF_LEN);
			snprintf(buf, XBUF_LEN, SECTION_DEVICE, N, DRIVER_INTEL, pci);
			fputs(buf, fp);	
			
			memset(&buf, 0, XBUF_LEN);
			snprintf(buf, XBUF_LEN, SECTION_SCREEN, N, N, N);
			fputs(buf, fp);
			
			N++;
		}
	}
	pci_cleanup(pciaccess);
	
	fputs(SECTION_LAYOUT, fp);
	
	for (int i = 0; i < N; i++) {
		memset(&buf, 0, XBUF_LEN);
		snprintf(buf, XBUF_LEN, "    Screen    %d   \"Screen%d\"\n", i, i);
		fputs(buf, fp);
	}
	fputs(SECTION_END, fp);
	
	fclose(fp);
    return 0;
}
