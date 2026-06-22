namespace ChannelState
{
    inline constexpr const char* NORMAL = "Normal";
    inline constexpr const char* BUSY = "Busy";
    inline constexpr const char* FULL = "Full";
    inline constexpr const char* DIE = "Die";
}

enum class E_ChannelState
{
    Normal = 0,
    Busy,
    Full,
    Die
};