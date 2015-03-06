namespace hpce{
	namespace utils {
		uint64_t shuffle64(unsigned bits, uint64_t x);
		void unpack_blob(unsigned w, unsigned h, unsigned bits, const uint64_t *pRaw, uint32_t *pUnpacked);
		void pack_blob(unsigned w, unsigned h, unsigned bits, const uint32_t *pUnpacked, uint64_t *pRaw);
		bool read_blob(int fd, uint64_t cbBlob, void *pBlob);
		void write_blob(int fd, uint64_t cbBlob, const void *pBlob);
		void invert(unsigned w, unsigned h, unsigned bits, std::vector<uint32_t> &pixels);
		void validate(int &argc, char *argv[], unsigned &w, unsigned &h, unsigned &bits, int &levels);
		// void validate(int &argc, char *argv[]);
	}
}