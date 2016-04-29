

.PHONY: common sha_commitment shared_signature timed_commitment cut_and_choose bitcoin/cpp bitcoin/java main


all:
	$(MAKE) -C common
	$(MAKE) -C sha_commitment
	$(MAKE) -C shared_signature
	$(MAKE) -C timed_commitment
	$(MAKE) -C cut_and_choose
	$(MAKE) -C bitcoin/cpp
	ant -buildfile bitcoin/java/
	$(MAKE) -C main

clean:
	$(MAKE) -C common clean
	$(MAKE) -C sha_commitment clean
	$(MAKE) -C shared_signature clean
	$(MAKE) -C timed_commitment clean
	$(MAKE) -C cut_and_choose clean
	$(MAKE) -C bitcoin/cpp clean
	ant -buildfile bitcoin/java/ clean
	$(MAKE) -C main clean
	$(MAKE) -C bitcoin-testnet-box clean




