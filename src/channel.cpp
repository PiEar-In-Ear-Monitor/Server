#include <boost/beast/core/detail/base64.hpp>
extern "C" {
    #include <libavutil/opt.h>
    #include <libavutil/channel_layout.h>
    #include <libavutil/samplefmt.h>
    #include <libswresample/swresample.h>
}
#include <sstream>
#include <string>
#include "channel.h"
#include "logger.h"

auto PiEar::channel::base64_encode(boost::beast::string_view s) -> std::string {
    auto data = reinterpret_cast<std::uint8_t const*> (s.data());
    std::size_t len = s.size();
    std::string dest;
    dest.resize(boost::beast::detail::base64::encoded_size(len));
    dest.resize(boost::beast::detail::base64::encode(&dest[0], data, len));
    return dest;
}
PiEar::channel::operator std::basic_string<char>() const {
    std::ostringstream s;
    s << "{";
    ADD_KEY_VAL_LIT(s, "piear_id", piear_id); COMMA(s);
    ADD_KEY_VAL_STR(s, "channel_name", channel_name); COMMA(s);
    ADD_KEY_VAL_LIT(s, "enabled", (enabled?"true":"false"));
    s << "}";
    return s.str();
}
PiEar::channel::operator bool() const {
    return enabled;
}
PiEar::channel::operator int() const {
    return piear_id;
}
auto PiEar::channel::operator==(int rhs) const -> bool {
    return piear_id == rhs;
}
auto PiEar::channel::add_sample(uint16_t *sample) -> void {
    buffer.push(sample);
}
auto PiEar::channel::get_sample() -> return_data {
    uint16_t *sample = buffer.pop();
    if (sample == nullptr) {
        return return_data{nullptr, 0};
    }
    auto *converted_sample = (uint16_t *) malloc(sizeof(uint16_t) * this->converted_sample_max); // TODO: Make some buffer pool for less malloc?
    if (converted_sample == nullptr) {
        PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::error) << "Failed to allocate memory for converted sample";
        throw std::runtime_error("Failed to allocate memory for converted sample");
    }
    if (swr_ctx == nullptr) {
        free(converted_sample);
        PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::error) << "swr_ctx is not initialized";
        throw std::runtime_error("swr_ctx is not initialized");
    }
    int frames = swr_convert(swr_ctx, (uint8_t **)&converted_sample, this->converted_sample_max, (const uint8_t **)&sample, BUFFER_CHUNK_SIZE);
    free(sample);
    if (frames < 0) {
        free(converted_sample);
        PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::error) << "no frames converted";
        throw std::runtime_error("no frames converted");
    }
    return return_data{converted_sample, frames};
}
auto PiEar::channel::setup_swr_ctx(int s_rate) -> void {
    sample_rate = s_rate;
    if (swr_ctx) {
        swr_free(&swr_ctx);
    }
    swr_ctx = swr_alloc();
    av_opt_set_int(swr_ctx, "in_channel_layout", AV_CH_LAYOUT_MONO, 0);
    av_opt_set_int(swr_ctx, "in_sample_rate", sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    av_opt_set_int(swr_ctx, "out_channel_layout", AV_CH_LAYOUT_MONO, 0);
    av_opt_set_int(swr_ctx, "out_sample_rate", FINAL_SAMPLE_RATE, 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    swr_init(swr_ctx);
    this->converted_sample_max = av_rescale_rnd(BUFFER_CHUNK_SIZE, FINAL_SAMPLE_RATE, sample_rate, AV_ROUND_UP);
}
auto PiEar::channel::swr_ctx_init() ->bool {
    return swr_ctx != nullptr;
}