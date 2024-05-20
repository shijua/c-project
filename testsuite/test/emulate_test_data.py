""" 
Data definitions for the emulator output state
"""
from dataclasses import dataclass
from enum import Enum, IntEnum
from io import StringIO
from pathlib import Path
import re
from typing import Dict, List, Optional, Set, Tuple, TypeVar

from result import Err, Ok, Result

###################################################################################################
""" PState """


@dataclass
class PState(dict):
    """Representation of an ARM64 CPU's PState flags"""

    N: bool = False
    Z: bool = False
    C: bool = False
    V: bool = False

    def pretty_str(self) -> str:
        printVal = lambda name: name if self.__dict__[name] else "-"
        return f"{printVal('N')}{printVal('Z')}{printVal('C')}{printVal('V')}"



###################################################################################################
""" ARM64 Register Setup """

NUM_REGS = 32
NUM_G_REGS = 31
_reg_names = [f"X{i:02}" for i in range(31)] + ["PC"]  # , "SP"]
Reg: Enum = IntEnum("Reg", {name: i for i, name in enumerate(_reg_names)})


def reg_to_str(reg: Reg) -> str:
    if reg == Reg.PC:
        return "PC"
    else:
        return f"X{reg.value:02}"

# def is_reg_name(name: str) -> bool:
#     is_g_reg = re.search(r"(?<=X)[0-9]{2}", name, re.IGNORECASE)
#     if not is_g_reg:
#         raise 
#     if is_g_reg.group(0):
#         return int(is_g_reg.group()) < NUM_REGS
#     return name in _reg_names


def str_to_reg(reg: str) -> Reg:
    return eval(f"Reg.{reg}")


###################################################################################################
""" Dict Utils """


def _compare_dicts(
    res: Dict, exp: Dict[str, int], act: Dict[str, int], string_keys: bool = False
) -> Tuple[bool, Dict[str, Dict[str, int]]]:
    """Compares two dictionaries and returns their difference in res"""

    for k in exp.keys() | act.keys():
        ks = str(k) if string_keys else k
        if k not in act:
            res[ks] = {"exp": exp[k], "act": 0}
        elif k not in exp:
            res[ks] = {"exp": 0, "act": act[k]}
        elif k in act and exp[k] != act[k]:
            res[ks] = {"exp": exp[k], "act": act[k]}
    return len(res) == 0, res


def compare_dicts(
    exp: Dict[str, int], act: Dict[str, int], string_keys: bool = False
) -> Tuple[bool, Dict[str, Dict[str, int]]]:
    """
    Compares two dictionaries and returns their difference, if any.

    Returns a tuple of (True, {}) if the dictionaries are equal,
    otherwise return (False, diff) where diff is a dictionary of the differences.
    """
    _compare_dicts({}, exp, act, string_keys)


###################################################################################################
""" CPU State """


@dataclass
class CPU_State:
    """Final CPU State"""

    regs: Dict[Reg, int]
    pstate: PState
    nz_mem: Dict[int, int]

    def dump(self, file: Path):
        """Write the CPU state to a file"""
        with open(file, "w") as f:
            f.write("Registers:\n")
            for reg in Reg:
                f.write(f"{reg.name:<7}= {self.regs[reg]:016x}\n")
            f.write(f"PSTATE : {self.pstate.pretty_str()}\n")
            f.write(f"Non-Zero Memory:\n")
            for addr in sorted(self.nz_mem.keys()):
                f.write(f"0x{addr:08x} : {self.nz_mem[addr]:08x}\n")

    # def __str__(self) -> str:
    #     sout = StringIO()
    #     self.dump(sout)
    #     return sout.getvalue()

    def compare(self, act: "CPU_State", string_keys: bool = False) -> Tuple[bool, Dict[str, Dict[str, int]]]:
        """
        Compares two CPU states and returns their difference, if any.

        Returns a tuple of (True, {}) if the states are equal,
        otherwise returns (False, diff) where diff is a dictionary of the differences.
        """
        difference: Dict[str, Dict[str, int]] = {"regs": {}, "nz_mem": {}}
        _compare_dicts(difference["regs"], self.regs, act.regs)
        difference["regs"] = {reg.name: diff for reg, diff in difference["regs"].items()}
        _compare_dicts(difference["nz_mem"], self.nz_mem, act.nz_mem, string_keys)

        if not ((self.pstate) == (act.pstate)):
            difference["pstate"] = {"exp": self.pstate.pretty_str(), "act": act.pstate.pretty_str()}

        any_diff = (
            len(difference["regs"]) > 0
            or len(difference["nz_mem"]) > 0
            or "pstate" in difference
        )
        return not any_diff, difference

T = TypeVar("T")

class ParseError():
    def __init__(self, msg: str):
        self.msg = msg
    def __str__(self):
        return f".out file parse error.\n{self.msg}"
    
class LineParseError(ParseError):
    def __init__(self, line: str, msg: str):
        self.line = line
        super().__init__(msg)
    def __str__(self):
        return f"{super}\n at line '{self.line}'"

class ParseErrors:
    class Reg(LineParseError):
        def __init__(self, line: str):
            msg = f"Invalid register line: '{line}'"
            super().__init__(line, msg)
    class RegVal(LineParseError):
        def __init__(self, line: str, val: str):
            msg = f"Invalid register value: 'val'"
            super().__init__(line, msg)
    class GRegName(LineParseError):
        def __init__(self, line: str, reg: str):
            msg = f"Invalid general register number: '{reg}'"
            super().__init__(line, msg)
    class NBitVal(LineParseError):
        def __init__(self, line, bitwidth: int, val: str):
            msg = f"{bitwidth}-bit value out of range: '{val}'"
            super().__init__(line, msg)
    class NotHex(LineParseError):
        def __init__(self, line: str, val: str):
            msg = f"Invalid hex value: '{val}'"
            super().__init__(line, msg)

    class RegName(LineParseError):
        def __init__(self, line: str, reg: str):
            msg = f"Invalid register name: '{reg}'"
            super().__init__(line, msg)
    class PStateLine(LineParseError):
        def __init__(self, line: str):
            msg = f"Invalid PState line: '{line}'"
            super().__init__(line, msg)
    class PStateLen(LineParseError):
        def __init__(self, line: str, pstate: str):
            msg = f"PState value string must be 4 characters long: '{pstate}'"
            super().__init__(line, msg)

    class PStateChar(LineParseError):
        def __init__(self, line: str, idx: int, ch: str):
            msg = f"PState string must be '-' or '{ch}' at index {idx}"
            super().__init__(line, msg)

    class DuplicateReg(LineParseError):
        def __init__(self, line: str, reg: str):
            msg = f"Duplicate register: '{reg}'"
            super().__init__(line, msg)
    
    class DuplicateNZMAddr(LineParseError):
        def __init__(self, line: str, addr: int):
            msg = f"Duplicate Non-zero memory address: '{addr}'"
            super().__init__(line, msg)
            
    class DuplicatePState(LineParseError):
        def __init__(self, line: str):
            msg = f"Duplicate PState line: '{line}'"
            super().__init__(line, msg)

    class NZMemLine(LineParseError):
        def __init__(self, line: str):
            msg = f"Invalid Non-zero memory line: '{line}'"
            super().__init__(line, msg)
    class NZMemAddrNotMultipleOf4(LineParseError):
        def __init__(self, line: str, addr: int):
            msg = f"Non-zero memory address must be a multiple of 4: '{addr}'"
            super().__init__(line, msg)
            
    class IgnoredLine(LineParseError):
        def __init__(self, line: str):
            msg = f"Ignored line: '{line}'"
            super().__init__(line, msg)

    class CPURegs(ParseError):
        def __init__(self, regs: Set[Reg]):
            missing_regs = set(Reg).difference(regs)
            extra_regs = regs.difference(set(Reg))
            missing = sorted(list(missing_regs))
            extra = sorted(list(extra_regs))
            self.msg = f"when Register Parsing. \nMissing registers: {str_reg_list(missing)}\nUnknown registers: {str_reg_list(extra)}"
    class CPU_PStateNotFound(ParseError):
        def __init__(self):
            super().__init__("No PState line found")
    class CPU_NZMem(ParseError):
        def __init__(self, msg: str):
            super().__init__(msg)
    



def parse_PState(line: str) -> Result[PState, LineParseError]:
    """ Parses a PState line of the form 'NZCV' where each character is either '-' or 'N', 'Z', 'C', or 'V' """
    toks = [x.strip() for x in line.split(":")]
    if len(toks) != 2:
        return Err(ParseErrors.PStateLine(line))
    _, pstate = toks
    if len(pstate) != 4:
        return Err(ParseErrors.PStateLen(line, pstate))

    def check_char(c: str, idx: int) -> Result[bool, LineParseError]:
        if pstate[idx] == "-": return Ok(False)
        if pstate[idx].lower() == c.lower(): return Ok(True)
        else: return Err(ParseErrors.PStateChar(line, idx, c)) 
    
    return result_tuple(
        check_char("n", 0), check_char("z", 1), check_char("c", 2), check_char("v", 3)
        ).and_then(lambda x: Ok(PState(*x)))


def parse_g_reg(line: str, reg_num: str) -> Result[Reg, LineParseError]:
    """ Parses a general register number of the form 'nn' """
    if reg_num.isdigit():
        val = int(reg_num)
        if 0 <= val < NUM_G_REGS:
            return Ok(Reg(val))
    return Err(ParseErrors.GRegName(line, reg_num))
    

def parse_reg(line: str, name: str) -> Result[Reg, LineParseError]:
    """ Parses a register name of the form 'Xnn' or 'PC' """
    is_g_reg = re.search(r"(?<=X)([0-3])?[0-9]$", name, re.IGNORECASE)
    is_pc = re.search(r"^PC$", name, re.IGNORECASE)
    if is_g_reg and is_g_reg.group(0):
        return parse_g_reg(line, is_g_reg.group(0))
    elif is_pc:
        return Ok(Reg.PC)
    else:
        return Err(ParseErrors.RegName(line, name))


def parse_n_bit_value(line: str, value: str, bitwidth: int) -> Result[int, LineParseError]:
    """ Parses a n-bit hex value """
    try:
        x = int(value, 16)
    except ValueError:
        return Err(ParseErrors.NotHex(line, value))
    if x < 0 or x > (2 ** bitwidth) - 1:
        return Err(ParseErrors.NBitVal(line, bitwidth, value))
    return Ok(x)


def result_tuple(*results: Result[T, LineParseError]) -> Result[Tuple[T, ...], LineParseError]:
    """ Combines multiple results into a single result """
    for result in results:
        if isinstance(result, Err):
            return result
    return Ok(tuple(result.unwrap() for result in results))

def parse_reg_value_line(line: str) -> Result[Tuple[Reg, int], LineParseError]:
    """ Parses a line of the form 'Xnn = 0xnnnnnnnnnnnnnnnn' """
    toks = [tok.strip() for tok in line.split("=")]
    if len(toks) != 2:
        return Err(ParseErrors.RegVal(f"Invalid register line: '{line}'"))
    reg, x = toks
    reg_val = parse_reg(line, reg)
    x_val = parse_n_bit_value(line, x, 64)

    return result_tuple(reg_val, x_val)

    
def parse_nzm_line(line: str) -> Result[Optional[Tuple[int, int]], LineParseError]:
    """ Parses a line of the form '0xnnnnnnnn : 0xnnnnnnnnnnnnnnnn' 
    
    Ok(None) when just a filler line
    """
    toks = [tok.strip() for tok in line.split(":")]
    if len(toks) != 2:
        return Err(ParseErrors.NZMemLine(line))
    addr, val = toks
    addr_val = parse_n_bit_value(line, addr, 64)
    
    if addr_val.is_err() \
        and isinstance(addr_val.err(), ParseErrors.NotHex) \
        and len(toks[1]) == 0:
        return Ok(None)
    if addr_val.is_ok() and addr_val.unwrap() % 4 != 0:
        addr_val = Err(ParseErrors.NZMemAddrNotMultipleOf4(line, addr_val.unwrap()))
    mem_val = parse_n_bit_value(line, val, 32)
    
    return result_tuple(addr_val, mem_val)
    

def parse_out_file(actState: Path) -> Result[CPU_State, List[ParseError]]:
    """Parses the custom `.out` file to a CPU_State object"""
    pstate: PState = None
    ignored: List[ParseErrors.IgnoredLine] = []
    regs: Dict[Reg, int] = {}
    nzmem: Dict[int, int] = {}
    errs: List[ParseError] = []
    with open(actState, "r") as f:
        for line in f.readlines():
            if line.isspace() or len(line) == 0:
                continue
            line = line.strip()
            if line.lower().startswith("pstate"):
                if pstate is not None:
                    errs.append(ParseErrors.DuplicatePState(line))
                    continue
                pstate = parse_PState(line)
            # Registers
            elif "=" in line:
                res = parse_reg_value_line(line)
                if res.is_err():
                    errs.append(res.unwrap_err())
                    continue
                else:
                    reg, val = res.ok()
                    if reg in regs:
                        errs.append(ParseErrors.DuplicateReg(line, reg_to_str(reg)))
                        continue
                    regs[reg] = val
            # Non-zero memory
            elif ":" in line:                
                res = parse_nzm_line(line)
                if res.is_err():
                    errs.append(res.unwrap_err())
                    continue
                elif res.ok() is None:
                    continue
                else:
                    addr, val = res.ok()
                    if addr in nzmem:
                        errs.append(ParseErrors.DuplicateNZMAddr(line, addr))
                        continue
                    nzmem[addr] = val
            else:
                ignored.append(ParseErrors.IgnoredLine(line))

        if pstate is None:
            errs.append(ParseErrors.CPU_PStateNotFound())
        elif pstate.is_err():
            errs.append(pstate.unwrap_err())
        else:
            pstate = pstate.unwrap()

        if set(regs.keys()) != set(Reg):
            errs.append(ParseErrors.CPURegs(set(regs.keys())))

        if len(errs) == 0:
            return Ok(CPU_State(regs, pstate, nzmem))
        else:
            return Err(errs + ignored)

def str_reg_list(regs: List[Reg]) -> str:
    return str([reg_to_str(reg) for reg in regs])
