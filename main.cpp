// PawnRandomix - Random Number Generator for Open.MP
// PRNG (PCG) dan CPRNG (ChaCha)
// Fixed: Modulo Bias & Float Consistency

#include <sdk.hpp>
#include <Server/Components/Pawn/pawn.hpp>
#include <Server/Components/Pawn/Impl/pawn_natives.hpp>
#include <Server/Components/Pawn/Impl/pawn_impl.hpp>

#include <cstdint>
#include <chrono>
#include <array>
#include <algorithm>
#include <limits>

// ==============================
// PCG Random Number Generator (PRNG)
// ==============================
class PCG32 {
private:
    uint64_t state;
    uint64_t inc;
    
    static constexpr uint64_t MULTIPLIER = 6364136223846793005ULL;
    static constexpr uint64_t INCREMENT = 1442695040888963407ULL;
    
public:
    PCG32(uint64_t seed = 0) {
        if (seed == 0) {
            seed = static_cast<uint64_t>(
                std::chrono::system_clock::now().time_since_epoch().count()
            );
        }
        
        state = 0;
        inc = (INCREMENT << 1u) | 1u;
        next_uint32();
        state += seed;
        next_uint32();
    }
    
    void seed(uint64_t seed) {
        state = 0;
        inc = (INCREMENT << 1u) | 1u;
        next_uint32();
        state += seed;
        next_uint32();
    }
    
    uint32_t next_uint32() {
        uint64_t oldstate = state;
        state = oldstate * MULTIPLIER + inc;
        
        uint32_t xorshifted = static_cast<uint32_t>(((oldstate >> 18u) ^ oldstate) >> 27u);
        uint32_t rot = static_cast<uint32_t>(oldstate >> 59u);
        
        return (xorshifted >> rot) | (xorshifted << ((~rot + 1) & 31u));
    }
    
    // Fixed: Consistent float generation menggunakan 2^32
    float next_float() {
        return static_cast<float>(next_uint32()) / 4294967296.0f;
    }
    
    // Fixed: Unbiased bounded random number generation
    uint32_t next_bounded(uint32_t bound) {
        if (bound == 0) return 0;
        
        // Lemire's method untuk unbiased random
        uint64_t m = static_cast<uint64_t>(next_uint32()) * static_cast<uint64_t>(bound);
        uint32_t leftover = static_cast<uint32_t>(m);
        
        if (leftover < bound) {
            uint32_t threshold = (0u - bound) % bound;
            while (leftover < threshold) {
                m = static_cast<uint64_t>(next_uint32()) * static_cast<uint64_t>(bound);
                leftover = static_cast<uint32_t>(m);
            }
        }
        
        return static_cast<uint32_t>(m >> 32);
    }
};

// ==============================
// ChaCha20 Random Number Generator (CPRNG)
// ==============================
class ChaChaRNG {
private:
    static constexpr int ROUNDS = 20;
    std::array<uint32_t, 16> state;
    uint32_t block[16];
    int position;
    
    static constexpr uint32_t CONSTANTS[4] = {
        0x61707865, 0x3320646e, 0x79622d32, 0x6b206574
    };
    
    static inline uint32_t rotl32(uint32_t x, int n) {
        return (x << n) | (x >> (32 - n));
    }
    
    void quarter_round(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
        a += b; d ^= a; d = rotl32(d, 16);
        c += d; b ^= c; b = rotl32(b, 12);
        a += b; d ^= a; d = rotl32(d, 8);
        c += d; b ^= c; b = rotl32(b, 7);
    }
    
    void generate_block() {
        std::copy(state.begin(), state.end(), block);
        
        for (int i = 0; i < ROUNDS; i += 2) {
            quarter_round(block[0], block[4], block[8], block[12]);
            quarter_round(block[1], block[5], block[9], block[13]);
            quarter_round(block[2], block[6], block[10], block[14]);
            quarter_round(block[3], block[7], block[11], block[15]);
            
            quarter_round(block[0], block[5], block[10], block[15]);
            quarter_round(block[1], block[6], block[11], block[12]);
            quarter_round(block[2], block[7], block[8], block[13]);
            quarter_round(block[3], block[4], block[9], block[14]);
        }
        
        for (int i = 0; i < 16; ++i) {
            block[i] += state[i];
        }
        
        state[12]++;
        position = 0;
    }
    
public:
    ChaChaRNG(uint64_t seed = 0) {
        if (seed == 0) {
            seed = static_cast<uint64_t>(
                std::chrono::system_clock::now().time_since_epoch().count()
            );
        }
        
        std::copy(CONSTANTS, CONSTANTS + 4, state.begin());
        
        uint32_t seed_parts[8];
        seed_parts[0] = static_cast<uint32_t>(seed);
        seed_parts[1] = static_cast<uint32_t>(seed >> 32);
        
        for (int i = 2; i < 8; ++i) {
            seed_parts[i] = seed_parts[i - 2] ^ (seed_parts[i - 1] << 13) ^ 0x9E3779B9u;
        }
        
        std::copy(seed_parts, seed_parts + 8, state.begin() + 4);
        
        state[12] = 0;
        state[13] = 0;
        state[14] = 0xDEADBEEF;
        state[15] = 0xCAFEBABE;
        
        position = 16;
    }
    
    void seed(uint64_t seed) {
        std::copy(CONSTANTS, CONSTANTS + 4, state.begin());
        
        uint32_t seed_parts[8];
        seed_parts[0] = static_cast<uint32_t>(seed);
        seed_parts[1] = static_cast<uint32_t>(seed >> 32);
        
        for (int i = 2; i < 8; ++i) {
            seed_parts[i] = seed_parts[i - 2] ^ (seed_parts[i - 1] << 13) ^ 0x9E3779B9u;
        }
        
        std::copy(seed_parts, seed_parts + 8, state.begin() + 4);
        
        state[12] = 0;
        state[13] = 0;
        state[14] = 0xDEADBEEF;
        state[15] = 0xCAFEBABE;
        
        position = 16;
    }
    
    uint32_t next_uint32() {
        if (position >= 16) {
            generate_block();
        }
        return block[position++];
    }
    
    // Fixed: Consistent float generation menggunakan 2^32
    float next_float() {
        return static_cast<float>(next_uint32()) / 4294967296.0f;
    }
    
    // Fixed: Unbiased bounded random number generation
    uint32_t next_bounded(uint32_t bound) {
        if (bound == 0) return 0;
        
        // Lemire's method untuk unbiased random
        uint64_t m = static_cast<uint64_t>(next_uint32()) * static_cast<uint64_t>(bound);
        uint32_t leftover = static_cast<uint32_t>(m);
        
        if (leftover < bound) {
            uint32_t threshold = (0u - bound) % bound;
            while (leftover < threshold) {
                m = static_cast<uint64_t>(next_uint32()) * static_cast<uint64_t>(bound);
                leftover = static_cast<uint32_t>(m);
            }
        }
        
        return static_cast<uint32_t>(m >> 32);
    }
};

// ==============================
// Main Randomix Component
// ==============================
class RandomixComponent final : public IComponent, public PawnEventHandler {
private:
    ICore* core = nullptr;
    IPawnComponent* pawn = nullptr;
    
    // Random generators
    PCG32 prng_generator;
    ChaChaRNG cprng_generator;
    
    // Global instance
    static RandomixComponent* g_RandomixComponent;
    
public:
    PROVIDE_UID(0x4D52616E646F6D69); // "MRandomi" in hex
    
    ~RandomixComponent() {
        g_RandomixComponent = nullptr;
    }
    
    StringView componentName() const override {
        return "PawnRandomix";
    }
    
    SemanticVersion componentVersion() const override {
        return SemanticVersion(1, 1, 0, 0);
    }
    
    void onLoad(ICore* c) override {
        core = c;
        
        // Seed dengan waktu sistem
        uint64_t seed = static_cast<uint64_t>(
            std::chrono::system_clock::now().time_since_epoch().count()
        );
        
        prng_generator = PCG32(seed);
        cprng_generator = ChaChaRNG(seed);
        
        g_RandomixComponent = this;
        
        core->printLn(" ");
        core->printLn("  PawnRandomix Component Loaded! [v1.1.0 - Fixed]");
        core->printLn("  PRNG: PCG32 (Fast, unbiased statistical)");
        core->printLn("  CPRNG: ChaCha20 (Cryptographically secure)");
        core->printLn("  Fixed: Modulo Bias + Float Consistency");
        core->printLn(" ");
    }
    
    void onInit(IComponentList* components) override {
        pawn = components->queryComponent<IPawnComponent>();
        
        if (pawn) {
            setAmxFunctions(pawn->getAmxFunctions());
            setAmxLookups(components);
            pawn->getEventDispatcher().addEventHandler(this);
        }
    }
    
    void onAmxLoad(IPawnScript& script) override {
        pawn_natives::AmxLoad(script.GetAMX());
    }
    
    void onAmxUnload(IPawnScript& script) override {}
    
    // PRNG functions (PCG) - Fixed: Menggunakan next_bounded
    uint32_t PRandom(uint32_t max) {
        if (max == 0) return 0;
        return prng_generator.next_bounded(max + 1);
    }
    
    uint32_t PRandRange(uint32_t min, uint32_t max) {
        if (min >= max) return min;
        uint32_t range = max - min;
        return min + prng_generator.next_bounded(range + 1);
    }
    
    float PRandFloatRange(float min, float max) {
        if (min >= max) return min;
        return min + prng_generator.next_float() * (max - min);
    }
    
    // CPRNG functions (ChaCha) - Fixed: Menggunakan next_bounded
    uint32_t CPRandom(uint32_t max) {
        if (max == 0) return 0;
        return cprng_generator.next_bounded(max + 1);
    }
    
    uint32_t CPRandRange(uint32_t min, uint32_t max) {
        if (min >= max) return min;
        uint32_t range = max - min;
        return min + cprng_generator.next_bounded(range + 1);
    }
    
    float CPRandFloatRange(float min, float max) {
        if (min >= max) return min;
        return min + cprng_generator.next_float() * (max - min);
    }
    
    // Seed functions
    void SeedPRNG(uint64_t seed) {
        prng_generator.seed(seed);
    }
    
    void SeedCPRNG(uint64_t seed) {
        cprng_generator.seed(seed);
    }
    
    // Advanced random functions
    bool PRandBool(float probability = 0.5f) {
        return prng_generator.next_float() < probability;
    }
    
    bool CPRandBool(float probability = 0.5f) {
        return cprng_generator.next_float() < probability;
    }
    
    uint32_t PRandWeighted(const uint32_t* weights, size_t count) {
        if (count == 0) return 0;
        
        uint32_t total = 0;
        for (size_t i = 0; i < count; i++) {
            total += weights[i];
        }
        
        if (total == 0) return 0;
        
        uint32_t rand = prng_generator.next_bounded(total);
        uint32_t sum = 0;
        
        for (size_t i = 0; i < count; i++) {
            sum += weights[i];
            if (rand < sum) return i;
        }
        
        return count - 1;
    }
    
    void PRandShuffle(uint32_t* array, size_t count) {
        for (size_t i = count - 1; i > 0; i--) {
            size_t j = prng_generator.next_bounded(i + 1);
            std::swap(array[i], array[j]);
        }
    }
    
    uint32_t PRandGaussian(float mean, float stddev) {
        // Box-Muller transform
        float u1 = prng_generator.next_float();
        float u2 = prng_generator.next_float();
        
        float z0 = sqrtf(-2.0f * logf(u1)) * cosf(6.28318530718f * u2);
        float result = mean + z0 * stddev;
        
        return static_cast<uint32_t>(result < 0 ? 0 : result);
    }
    
    void PRandPosition2D(float centerX, float centerY, float radius, float& outX, float& outY) {
        float angle = prng_generator.next_float() * 6.28318530718f;
        float r = sqrtf(prng_generator.next_float()) * radius;
        
        outX = centerX + r * cosf(angle);
        outY = centerY + r * sinf(angle);
    }
    
    void PRandPosition3D(float centerX, float centerY, float centerZ, float radius, 
                         float& outX, float& outY, float& outZ) {
        float theta = prng_generator.next_float() * 6.28318530718f;
        float phi = acosf(2.0f * prng_generator.next_float() - 1.0f);
        float r = powf(prng_generator.next_float(), 1.0f/3.0f) * radius;
        
        float sinPhi = sinf(phi);
        outX = centerX + r * sinPhi * cosf(theta);
        outY = centerY + r * sinPhi * sinf(theta);
        outZ = centerZ + r * cosf(phi);
    }
    
    uint32_t PRandDice(uint32_t sides, uint32_t count = 1) {
        if (sides == 0 || count == 0) return 0;
        
        uint32_t total = 0;
        for (uint32_t i = 0; i < count; i++) {
            total += prng_generator.next_bounded(sides) + 1;
        }
        return total;
    }
    
    uint32_t CPRandToken(uint32_t length) {
        // Generate cryptographic token (for session IDs, etc)
        uint32_t token = 0;
        for (uint32_t i = 0; i < length && i < 8; i++) {
            token = (token << 4) | (cprng_generator.next_bounded(16));
        }
        return token;
    }
    
    void reset() override {
        // Re-seed dengan waktu baru
        uint64_t seed = static_cast<uint64_t>(
            std::chrono::system_clock::now().time_since_epoch().count()
        );
        
        prng_generator.seed(seed);
        cprng_generator.seed(seed);
    }
    
    void free() override {
        delete this;
    }
    
    void onReady() override {}
    
    void onFree(IComponent* component) override {
        if (component == pawn) {
            pawn = nullptr;
            setAmxFunctions();
            setAmxLookups();
        }
    }
    
    // Static getter for the global instance
    static RandomixComponent* GetInstance() {
        return g_RandomixComponent;
    }
};

// Initialize static member
RandomixComponent* RandomixComponent::g_RandomixComponent = nullptr;

// ==============================
// Component Entry Point
// ==============================
COMPONENT_ENTRY_POINT() {
    return new RandomixComponent();
}

// ==============================
// Helper Functions
// ==============================
static inline RandomixComponent* GetRandomixComponent() {
    return RandomixComponent::GetInstance();
}

// ==============================
// Pawn Native Functions
// ==============================

// PRNG functions (PCG) - Fast, statistical random
SCRIPT_API(PRandom, int(int max)) {
    auto component = GetRandomixComponent();
    if (!component || max < 0) return 0;
    return static_cast<int>(component->PRandom(static_cast<uint32_t>(max)));
}

SCRIPT_API(PRandRange, int(int min, int max)) {
    auto component = GetRandomixComponent();
    if (!component) return min;
    
    if (min > max) std::swap(min, max);
    return static_cast<int>(component->PRandRange(
        static_cast<uint32_t>(min),
        static_cast<uint32_t>(max)
    ));
}

SCRIPT_API(PRandFloatRange, float(float min, float max)) {
    auto component = GetRandomixComponent();
    if (!component) return min;
    
    if (min > max) std::swap(min, max);
    return component->PRandFloatRange(min, max);
}

// CPRNG functions (ChaCha) - Cryptographically secure
SCRIPT_API(CPRandom, int(int max)) {
    auto component = GetRandomixComponent();
    if (!component || max < 0) return 0;
    return static_cast<int>(component->CPRandom(static_cast<uint32_t>(max)));
}

SCRIPT_API(CPRandRange, int(int min, int max)) {
    auto component = GetRandomixComponent();
    if (!component) return min;
    
    if (min > max) std::swap(min, max);
    return static_cast<int>(component->CPRandRange(
        static_cast<uint32_t>(min),
        static_cast<uint32_t>(max)
    ));
}

SCRIPT_API(CPRandFloatRange, float(float min, float max)) {
    auto component = GetRandomixComponent();
    if (!component) return min;
    
    if (min > max) std::swap(min, max);
    return component->CPRandFloatRange(min, max);
}

// Seed functions
SCRIPT_API(SeedPRNG, int(int seed)) {
    auto component = GetRandomixComponent();
    if (!component) return 0;
    
    component->SeedPRNG(static_cast<uint64_t>(seed));
    return 1;
}

SCRIPT_API(SeedCPRNG, int(int seed)) {
    auto component = GetRandomixComponent();
    if (!component) return 0;
    
    component->SeedCPRNG(static_cast<uint64_t>(seed));
    return 1;
}

// ==============================
// Helper Functions for Array Access
// ==============================
static inline cell* GetArrayPtr(AMX* amx, cell param)
{
    cell* addr = nullptr;
    amx_GetAddr(amx, param, &addr);
    return addr;
}

// ==============================
// Advanced Native Functions
// ==============================

// Random boolean dengan probability
SCRIPT_API(PRandBool, bool(float probability)) {
    auto component = GetRandomixComponent();
    if (!component) return false;
    
    if (probability <= 0.0f) return false;
    if (probability >= 1.0f) return true;
    
    return component->PRandBool(probability);
}

SCRIPT_API(CPRandBool, bool(float probability)) {
    auto component = GetRandomixComponent();
    if (!component) return false;
    
    if (probability <= 0.0f) return false;
    if (probability >= 1.0f) return true;
    
    return component->CPRandBool(probability);
}

// Weighted random selection
SCRIPT_API(PRandWeighted, int(cell weightsAddr, int count)) {
    auto component = GetRandomixComponent();
    if (!component || count <= 0) return 0;
    
    cell* weights = GetArrayPtr(GetAMX(), weightsAddr);
    if (!weights) return 0;
    
    uint32_t* uweights = new uint32_t[count];
    for (int i = 0; i < count; i++) {
        uweights[i] = weights[i] > 0 ? static_cast<uint32_t>(weights[i]) : 0;
    }
    
    uint32_t result = component->PRandWeighted(uweights, count);
    delete[] uweights;
    
    return static_cast<int>(result);
}

// Shuffle array in-place
SCRIPT_API(PRandShuffle, bool(cell arrayAddr, int count)) {
    auto component = GetRandomixComponent();
    if (!component || count <= 0) return false;
    
    cell* array = GetArrayPtr(GetAMX(), arrayAddr);
    if (!array) return false;
    
    uint32_t* uarray = new uint32_t[count];
    for (int i = 0; i < count; i++) {
        uarray[i] = static_cast<uint32_t>(array[i]);
    }
    
    component->PRandShuffle(uarray, count);
    
    for (int i = 0; i < count; i++) {
        array[i] = static_cast<int>(uarray[i]);
    }
    
    delete[] uarray;
    return true;
}

// Gaussian/Normal distribution
SCRIPT_API(PRandGaussian, int(float mean, float stddev)) {
    auto component = GetRandomixComponent();
    if (!component) return static_cast<int>(mean);
    
    return static_cast<int>(component->PRandGaussian(mean, stddev));
}

// Random position in 2D circle
SCRIPT_API(PRandPosition2D, bool(float centerX, float centerY, float radius, float& outX, float& outY)) {
    auto component = GetRandomixComponent();
    if (!component || radius <= 0.0f) return false;
    
    component->PRandPosition2D(centerX, centerY, radius, outX, outY);
    return true;
}

// Random position in 3D sphere
SCRIPT_API(PRandPosition3D, bool(float centerX, float centerY, float centerZ, float radius, 
                                  float& outX, float& outY, float& outZ)) {
    auto component = GetRandomixComponent();
    if (!component || radius <= 0.0f) return false;
    
    component->PRandPosition3D(centerX, centerY, centerZ, radius, outX, outY, outZ);
    return true;
}

// Roll dice (D&D style)
SCRIPT_API(PRandDice, int(int sides, int count)) {
    auto component = GetRandomixComponent();
    if (!component || sides <= 0 || count <= 0) return 0;
    
    return static_cast<int>(component->PRandDice(
        static_cast<uint32_t>(sides),
        static_cast<uint32_t>(count)
    ));
}

// Cryptographic token generation
SCRIPT_API(CPRandToken, int(int length)) {
    auto component = GetRandomixComponent();
    if (!component || length <= 0) return 0;
    
    return static_cast<int>(component->CPRandToken(static_cast<uint32_t>(length)));
}