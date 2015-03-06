namespace hpce{
	namespace standard {
		uint32_t vmin(uint32_t a, uint32_t b);
		uint32_t vmin(uint32_t a, uint32_t b, uint32_t c);
		uint32_t vmin(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
		uint32_t vmin(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
		void erode(unsigned w, unsigned h, const std::vector<uint32_t> &input, std::vector<uint32_t> &output);
		uint32_t vmax(uint32_t a, uint32_t b);
		uint32_t vmax(uint32_t a, uint32_t b, uint32_t c);
		uint32_t vmax(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
		uint32_t vmax(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
		void dilate(unsigned w, unsigned h, const std::vector<uint32_t> &input, std::vector<uint32_t> &output);
		void process(int levels, unsigned w, unsigned h, unsigned /*bits*/, std::vector<uint32_t> &pixels);
		


		int go(unsigned w, unsigned h, unsigned bits, int levels);

	}
}