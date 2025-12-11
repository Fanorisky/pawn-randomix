# open.mp Easing-Functions Component

A lightweight open.mp server component that adds signed 64-bit integer support to Pawn scripts. It exposes a `BigInt` tag plus a collection of arithmetic, conversion, and comparison natives so game-mode authors can safely store counters such as bank balances, experience totals, and economy statistics without overflowing the default 32-bit cell size.

## Installation

1. Visit the **Releases** page of this repository and download the archive or binary that matches your server platform (Windows `.dll`, Linux `.so`).
2. Extract the download and copy the component file into your open.mp server `components/` directory.
3. Copy `omp_bigint.inc` into the include directory used by your Pawn compiler (e.g., `gamemodes/include/`).
4. Add `#include <omp_bigint>` to any script that needs the BigInt natives and rebuild your Pawn scripts.

## Usage

The include defines the `BigInt` tag plus helpers such as `new_bigint`, `BigInt_SetInt`, and the arithmetic/compare natives. A quick example that tracks per-player bank balances:

```pawn
#include <omp_bigint>

new_bigint(g_BankBalance[MAX_PLAYERS]);

/*
 Or if you want to use the common way of creating variables:
 
     new BigInt:g_BankBalance[MAX_PLAYERS][eBigIntParts];

     new BigInt:TestVar[eBigIntParts];
 or
     new_bigint(TestVar);

 or in your enum:
     enum PLAYER_DATA
     {
        ...
        BigInt:BankMoney[eBigIntParts],
        ...
     };
*/
 

public OnPlayerConnect(playerid)
{
    BigInt_SetInt(g_BankBalance[playerid], 0);
}

stock BankDeposit(playerid, amount)
{
    BigInt_AddInt(g_BankBalance[playerid], amount);
}

CMD:balance(playerid)
{
    new buf[32];
    BigInt_ToString(g_BankBalance[playerid], buf, sizeof buf);
    SendClientMessage(playerid, -1, buf);
    return 1;
}
```

All operations return simple success/failure flags (e.g., division by zero yields `false`) so you can guard your logic as needed.

## Contributing

Contributions are welcome! To get started:

- Fork the repository and create a branch for your change.
- Keep commits focused; include tests or Pawn snippets that demonstrate new or fixed behavior when possible.
- Run the CMake build locally to ensure the component still compiles on your platform.
- Open a pull request against `master`, describing the motivation, implementation details, and any testing performed.

If you encounter issues or have feature ideas, feel free to open a GitHub issue with as much detail as possible.
