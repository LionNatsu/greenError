#include "SDL/SDL_image.h"
#include "jpeglib.h"

class JpegCompress {
public:
  jpeg_compress_struct *cinfo;
  JOCTET *buffer;
private:
  static void init_buffer(jpeg_compress_struct* cinfo) {};
  static boolean empty_buffer(jpeg_compress_struct* cinfo) {return TRUE;};
  static void term_buffer(jpeg_compress_struct* cinfo) {};
public:
  JpegCompress(SDL_Surface *image) {
    this->buffer = new JOCTET[image->w * image->h *3];
    this->cinfo = new jpeg_compress_struct;

    auto *jerr = new jpeg_error_mgr;
    cinfo->err = jpeg_std_error(jerr);
    jpeg_create_compress(cinfo);

    auto *jdest = new jpeg_destination_mgr;
    jdest->init_destination    = this->init_buffer;
    jdest->empty_output_buffer = this->empty_buffer;
    jdest->term_destination    = this->term_buffer;
    jdest->next_output_byte    = buffer;
    jdest->free_in_buffer      = image->w * image->h *3;

    this->cinfo->dest = jdest;
    this->cinfo->image_width      = image->w;
    this->cinfo->image_height     = image->h;
    this->cinfo->input_components = 3;
    this->cinfo->in_color_space   = JCS_YCbCr;

    jpeg_set_defaults(this->cinfo);
    this->cinfo->optimize_coding  = TRUE;

    return;
  };
  ~JpegCompress() {
    delete this->cinfo->err;
    delete this->cinfo->dest;
    delete []this->buffer;
    delete this->cinfo;
  };
  void Compress(uint8_t *pointer, int quality, J_DCT_METHOD dct) {
    jpeg_set_quality (this->cinfo, 100, true);
    this->cinfo->dct_method = dct;
    jpeg_start_compress(this->cinfo, true);
    while (this->cinfo->next_scanline < this->cinfo->image_height) {
      JSAMPROW row_pointer = (JSAMPROW) &pointer[this->cinfo->next_scanline * this->cinfo->image_width * 3];
      jpeg_write_scanlines(this->cinfo, &row_pointer, 1);
    }
    jpeg_finish_compress(this->cinfo);
  };
  size_t GetSize() {
    return this->cinfo->dest->next_output_byte - this->buffer;
  };
  JOCTET *GetBuffer() {
    return this->buffer;
  };
};
