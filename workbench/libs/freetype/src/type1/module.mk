make_module_list: add_type1_driver

add_type1_driver:
	$(OPEN_DRIVER)t1_driver_interface$(CLOSE_DRIVER)
	$(ECHO_DRIVER)type1     $(ECHO_DRIVER_DESC) Postscript font files with extension *.pfa or *.pfb $(ECHO_DRIVER_DONE)

