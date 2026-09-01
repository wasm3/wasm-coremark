# Copyright 2018 Embedded Microprocessor Benchmark Consortium (EEMBC)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Original Author: Shay Gal-on

# The POSIX port as-is, which is what WASI gives us: clock_gettime, malloc,
# printf and an argv. Only three things differ.

# clock_gettime lives in wasi-libc proper; there is no librt to link.
NO_LIBRT = 1

include posix/core_portme.mak

# posix leaves MEM_LOCATION as a "fill this in" placeholder, which then gets
# printed in the report. MEM_METHOD is MEM_MALLOC here.
CFLAGS += -DMEM_LOCATION='"Heap"'

# Nothing reads the name section in a benchmark binary.
LFLAGS_END += -Wl,--strip-all
