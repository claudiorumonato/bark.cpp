#include <iostream>
#include <iomanip>
#include <tuple>

#include "bark.h"
#include "common.h"
#include "ggml.h"

int main(int argc, char **argv) {
    ggml_time_init();
    const int64_t t_main_start_us = ggml_time_us();

	ggml_backend_load_all();

    bark_params params;
    bark_verbosity_level verbosity = bark_verbosity_level::LOW;

    if (bark_params_parse(argc, argv, params) > 0) {
        fprintf(stderr, "%s: Could not parse arguments\n", __func__);
        return 1;
    }

    std::cout << R"(    __               __                          )"
              << "\n"
              << R"(   / /_  ____ ______/ /__        _________  ____ )"
              << "\n"
              << R"(  / __ \/ __ `/ ___/ //_/       / ___/ __ \/ __ \)"
              << "\n"
              << R"( / /_/ / /_/ / /  / ,<    _    / /__/ /_/ / /_/ /)"
              << "\n"
              << R"(/_.___/\__,_/_/  /_/|_|  (_)   \___/ .___/ .___/ )"
              << "\n"
              << R"(                                  /_/   /_/      )"
              << "\n";

	bark_context_params ctx_params = bark_context_default_params();

    ctx_params.verbosity = verbosity;
    ctx_params.progress_callback = nullptr;
    ctx_params.progress_callback_user_data = nullptr;
	
    // initialize bark context
    struct bark_context *bctx = bark_load_model(params.model_path.c_str(), ctx_params, params.seed, true);
    if (!bctx) {
        fprintf(stderr, "%s: Could not load model\n", __func__);
        exit(1);
    }

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();	
	
    // generate audio
    if (!bark_generate_audio(bctx, params.prompt.c_str(), params.n_threads)) {
        fprintf(stderr, "%s: An error occured. If the problem persists, feel free to open an issue to report it.\n", __func__);
        exit(1);
    }

    const float *audio_data = bark_get_audio_data(bctx);
    if (audio_data == nullptr) {
        fprintf(stderr, "%s: Could not get audio data\n", __func__);
        exit(1);
    }

    const int audio_arr_size = bark_get_audio_data_size(bctx);

    std::cout << "bark_generate_audio: " << std::setprecision(2) << ((audio_arr_size + 23) / 24) << " ms of audio generated in " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() << " ms" << std::endl;

    std::vector<float> audio_arr(audio_data, audio_data + audio_arr_size);
	
	if (!params.out_path.empty()) {
		fprintf(stdout, "writing to %s ...\n", params.out_path.c_str());
		write_wav_on_disk(audio_arr, params.out_path);
	}
	
	if (params.play) {
		printf("Playing audio %.2fs ...\n", (float)audio_arr.size() / SAMPLE_RATE);
        play_fp_data(audio_arr.data(), audio_arr.size(), SAMPLE_RATE, 1);
	}

    // report timing
    {
        const int64_t t_main_end_us = ggml_time_us();

        printf("\n\n");
        /*printf("%s:     load time = %8.2f ms\n", __func__, bctx->t_load_us / 1000.0f);
        printf("%s:     eval time = %8.2f ms\n", __func__, bctx->t_eval_us / 1000.0f);*/
        printf("%s:    total time = %8.2f ms\n", __func__, (t_main_end_us - t_main_start_us) / 1000.0f);
    }

    bark_free(bctx);

    return 0;
}
