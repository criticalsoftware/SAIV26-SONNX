# ==========================================================
# Makefile for formal verification of the 'reshape' operation
# This Makefile is executed from: operators/reshape/formal
# ==========================================================

.PHONY: lib clean

# Directories 
CODE_DIR = ../code
CDRIVER_DIR = ../../../tensor/drivers
LIBTENSOR_DRIVER_DIR = ../../../tensor/drivers

# ==========================================================
# TARGETS
# ==========================================================	
EXTRACT_LIB = -D c -D $(CDRIVER_DIR)/cdriver.drv -L . --modular --interface -o $(CODE_DIR) 
EXTRACT_WHERE = -D c -D reshape_driver.drv -D $(LIBTENSOR_DRIVER_DIR)/libtensor.drv -L . --modular --interface -o $(CODE_DIR)

lib:
	@rm -fr $(CODE_DIR)
	@why3 extract $(EXTRACT_LIB) libtensor.CTensor
	@why3 extract $(EXTRACT_LIB) libtensor.CTensorInt64
	@why3 extract $(EXTRACT_LIB) libvector.CIndex
	@why3 extract $(EXTRACT_WHERE) reshape.COPReshape
	@cd $(CODE_DIR) && gcc -c *.c
	@find $(CODE_DIR) -type f | sort

clean:
	rm -rf $(CODE_DIR)/*.o