# ==========================================================
# Makefile for Tensor Library - Code Extraction
# 
# Extracts verified C code from Why3 modules (libvector, libtensor)
# Output: ../code/ directory C files
# ==========================================================

.PHONY: lib clean

EXTRACT=-D c -D ../drivers/cdriver.drv  --modular --interface -o ../code

# ==========================================================
# TARGETS
# ==========================================================

lib:
	@echo "------------------------------------------"
	@echo "--- Extraction"
	@echo "------------------------------------------"
	@rm -fr ../code
	@why3 extract $(EXTRACT) libvector.CIndex
	@why3 extract $(EXTRACT) libtensor.CTensor
	@why3 extract $(EXTRACT) libtensor.CTensorInt64
	@cd ../code && gcc -c *.c
	@find ../code -type f | sort

clean:
	rm -fr ../code