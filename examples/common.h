#include <string>
#include <thread>
#include <vector>

#define BARK_DEFAULT_MIN_THREADS	4
#define BARK_DEFAULT_MAX_THREADS	((int) std::thread::hardware_concurrency())
#define MIN_AB(A,B)                 ((A)<(B)?(A):(B))
#define BARK_DEFAULT_N_THREADS      MIN_AB(BARK_DEFAULT_MIN_THREADS,BARK_DEFAULT_MAX_THREADS)

#define SAMPLE_RATE 24000

struct bark_params {
    int32_t n_threads = BARK_DEFAULT_N_THREADS;	// Number of threads used for audio generation.
    std::string prompt;							// User prompt.
    std::string model_path;						// Location of model weights.
    std::string out_path;						// Destination path for generated WAV file.
    int use_gpu = 0;							// Layers to offload to GPU
    int32_t seed = 0;							// Seed for reproducibility in token sampling.
	int play = -1;								// Play audio
};

struct bark_context;

std::vector<float> bark_generated_audio(bark_context *bctx);

/**
 * @brief Writes a WAV file from disk and stores the audio data in a vector of floats.
 *
 * @param in_path Path to the input WAV file.
 * @param audio_arr Vector to store the audio data.
 * @return true If the file was successfully read.
 * @return false If the file could not be read.
 */
void write_wav_on_disk(std::vector<float>& audio_arr, std::string dest_path);

/**
 * @brief Play an array of samples as a mono audio stream at given sample rate.
 * @param samples Pointer to audio frames.
 * @param frameCount Number of frames to play.
 * @param sample_rate Sample rate.
 * @param channels Number of channels (1).
 * @return true If the file was successfully read.
 */
int play_fp_data(const float* samples, size_t frameCount, int sample_rate, int channels = 1);

/**
 * @brief Parses command line arguments and stores them in a bark_params struct.
 *
 * @param argc The number of command line arguments.
 * @param argv An array of C-strings containing the command line arguments.
 * @param params A reference to a bark_params struct where the parsed arguments will be stored.
 * @return int Returns 0 if the parsing was successful, otherwise returns a non-zero value.
 */
int bark_params_parse(int argc, char** argv, bark_params& params);

/**
 * Prints the usage information for the bark command-line tool.
 *
 * @param argv The command-line arguments passed to the program.
 * @param params The parameters used by the bark command-line tool.
 */
void bark_print_usage(char** argv, const bark_params& params);
