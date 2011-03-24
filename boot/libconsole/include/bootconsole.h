struct multiboot;
struct vbe_mode;

extern void	    *scr_FrameBuffer;	/* VRAM address, must be set    */
extern unsigned int  scr_Width;		/* Display width in characters	*/
extern unsigned int  scr_Height;	/* Display height in characters */

/*
 * Address of mirror buffer for graphical console.
 * Must be set if you want to use it.
 */
extern char *fb_Mirror;

/* Common initialization */
void con_InitMultiboot(struct multiboot *mb);
void con_InitVESA(unsigned short version, struct vbe_mode *mode);
void con_InitVGA(void);
void con_InitSerial(char *cmdline);

/* Common output */
void con_Clear(void);
void con_Putc(char chr);

/* Text output */
void txt_Clear(void);
void txt_Putc(char chr);

/* Graphical framebuffer output */
void fb_Init(unsigned int width, unsigned int height, unsigned int depth, unsigned int pitch);
void fb_Clear(void);
void fb_Putc(char chr);

/* Serial I/O */
void serial_Init(char *opts);
void serial_Putc(unsigned char data);
