#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "common.h"

void write_wav_on_disk(std::vector<float>& audio_arr, std::string dest_path) {
    drwav_data_format format;
    format.bitsPerSample = 32;
    format.sampleRate = SAMPLE_RATE;
    format.container = drwav_container_riff;
    format.channels = 1;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT;

    drwav wav;
    drwav_init_file_write(&wav, dest_path.c_str(), &format, NULL);
    drwav_uint64 frames = drwav_write_pcm_frames(&wav, audio_arr.size(), audio_arr.data());
    drwav_uninit(&wav);

    fprintf(stderr, "%s: Number of frames written = %lld.\n", __func__, frames);
}

void bark_print_usage(char** argv, const bark_params& params) {
    std::cout << "usage: " << argv[0] << " [options]\n"
              << "\n"
              << "options:\n"
              << "  -h, --help            show this help message and exit\n"
			  << "  -g					  use GPU\n"
              << "  -t N, --threads N     number of threads to use during computation (default: " << params.n_threads << ")\n"
              << "  -s N, --seed N        seed for random number generator (default: " << params.seed << ")\n"
              << "  -i PROMPT, --prompt PROMPT\n"
              << "                        prompt to start generation with (default: random)\n"
              << "  -m FILE, --model FILE model path (default: " << params.model_path << ")\n"
			  << "  -p					  play generater audio\n"
              << "  -o FILE, --out FILE   output generated wav (default: " << params.out_path << ")\n"
              << "\n";
}

int bark_params_parse(int argc, char** argv, bark_params& params) {
	
	int i = 1;
    for (; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-t" || arg == "--threads") {
            params.n_threads = std::stoi(argv[++i]);
        } else if (arg == "-i" || arg == "--prompt") {
            params.prompt = argv[++i];
        } else if (arg == "-m" || arg == "--model_path") {
            params.model_path = argv[++i];
        } else if (arg == "-g" || arg == "--gpu") {
            params.use_gpu = true;
        } else if (arg == "-p" || arg == "--play") {
            params.play = 0;
        } else if (arg == "-s" || arg == "--seed") {
            params.seed = std::stoi(argv[++i]);
        } else if (arg == "-o" || arg == "--out") {
            params.out_path = argv[++i];
        } else if (arg == "-h" || arg == "--help") {
            bark_print_usage(argv, params);
            exit(0);
        } else {
			if (arg[0] == '-') {
				fprintf(stderr, "error: unknown argument: %s\n", arg.c_str());
				bark_print_usage(argv, params);
				exit(0);
			}
			break;
        }
    }
	
	if (params.prompt.empty() && i < argc) {
		std::ostringstream oss;
		oss << argv[i ++];
		for (; i < argc; i++) {
			oss << ' ' << argv[i ++];
		}
		params.prompt = oss.str();
	}

    return 0;
}

static const float* g_samples      = NULL;
static ma_uint64    g_frameCount   = 0;      /* Numero totale di frame nel buffer. */
static ma_uint64    g_readCursor   = 0;      /* Posizione corrente di lettura.     */

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    float* out = (float*)pOutput;
    (void)pInput;
    ma_uint32 i;

    for (i = 0; i < frameCount; ++i) {
        float sample = 0.0f;

        if (g_readCursor < g_frameCount) {
            sample = g_samples[g_readCursor++];
        } else {
            sample = 0.0f; /* Una volta finiti i dati mandi silenzio. */
        }

        out[i] = sample;
    }
}

int play_fp_data(const float* samples, ma_uint64 frameCount, int sample_rate, int channels)
{
    ma_result       result;
    ma_device_config config;
    ma_device        device;

    g_samples    = samples;
    g_frameCount = frameCount;
    g_readCursor = 0;

    config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = ma_format_f32;   /* float32.             [web:7] */
    config.playback.channels = channels;        /* mono.                [web:7] */
    config.sampleRate        = sample_rate;     /* 24 kHz.              [web:7] */
    config.dataCallback      = data_callback;   /* tua callback.        [web:5] */
    config.pUserData         = NULL;

    result = ma_device_init(NULL, &config, &device); /* default playback device. [web:5] */
    if (result != MA_SUCCESS) {
        return -1;
    }

    ma_engine engine;
    ma_engine_config engine_config;
    /* Now that we have the device we can initialize the engine. The device is passed into the engine's config. */
    engine_config = ma_engine_config_init();
    engine_config.pDevice          = &device;
    engine_config.noAutoStart      = MA_TRUE;    /* Don't start the engine by default - we'll do that manually below. */

    result = ma_engine_init(&engine_config, &engine);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize engine.\n");
        return -1;
    }

    result = ma_engine_start(&engine);
    if (result != MA_SUCCESS) {
        printf("WARNING: Failed to start engine.\n");
    } else {
        if ((result = ma_engine_set_volume(&engine, 0.3f)) != MA_SUCCESS)
            printf("%s: ma_engine_set_volume: %s.\n", __func__, ma_result_description(result));
    }


    result = ma_device_start(&device); /* Inizia la riproduzione. [web:5] */
    if (result != MA_SUCCESS) {
        ma_device_uninit(&device);
        return -2;
    }

    /* Bloccati finché non hai finito di riprodurre.
       Qui metto un busy wait stupido giusto per esempio. */
    while (g_readCursor < g_frameCount) {
        ma_sleep(10); /* 10 ms. [web:7] */
    }

    ma_device_uninit(&device);
    return 0;
}
