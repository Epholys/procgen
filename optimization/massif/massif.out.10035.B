desc: --time-unit=B
cmd: ./procgen.out 14
time_unit: B
#-----------
snapshot=0
#-----------
time=0
mem_heap_B=0
mem_heap_extra_B=0
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=1
#-----------
time=8427128
mem_heap_B=822453
mem_heap_extra_B=739
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=2
#-----------
time=12417120
mem_heap_B=1215664
mem_heap_extra_B=720
mem_stacks_B=0
heap_tree=detailed
n4: 1215664 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 786432 0x11036B: void std::vector<char, std::allocator<char> >::_M_range_insert<__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > > >(__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, std::forward_iterator_tag) (new_allocator.h:104)
  n1: 786432 0x10FBAF: lsys::LSystem::iter(int) (stl_vector.h:1375)
   n1: 786432 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
    n0: 786432 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 354293 0x10FD07: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 354293 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 354293 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 72704 0x579E9BE: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.22)
  n1: 72704 0x400F8A8: call_init.part.0 (dl-init.c:72)
   n1: 72704 0x400F9B9: _dl_init (dl-init.c:30)
    n1: 72704 0x4000C58: ??? (in /lib/x86_64-linux-gnu/ld-2.24.so)
     n1: 72704 0x0: ???
      n1: 72704 0xFFF00074D: ???
       n0: 72704 0xFFF00075B: ???
 n0: 2235 in 27 places, all below massif's threshold (1.00%)
#-----------
snapshot=3
#-----------
time=18258640
mem_heap_B=2002096
mem_heap_extra_B=720
mem_stacks_B=0
heap_tree=detailed
n4: 2002096 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 1572864 0x11036B: void std::vector<char, std::allocator<char> >::_M_range_insert<__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > > >(__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, std::forward_iterator_tag) (new_allocator.h:104)
  n1: 1572864 0x10FBAF: lsys::LSystem::iter(int) (stl_vector.h:1375)
   n1: 1572864 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
    n0: 1572864 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 354293 0x10FD07: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 354293 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 354293 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 72704 0x579E9BE: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.22)
  n1: 72704 0x400F8A8: call_init.part.0 (dl-init.c:72)
   n1: 72704 0x400F9B9: _dl_init (dl-init.c:30)
    n1: 72704 0x4000C58: ??? (in /lib/x86_64-linux-gnu/ld-2.24.so)
     n1: 72704 0x0: ???
      n1: 72704 0xFFF00074D: ???
       n0: 72704 0xFFF00075B: ???
 n0: 2235 in 27 places, all below massif's threshold (1.00%)
#-----------
snapshot=4
#-----------
time=22695104
mem_heap_B=2710684
mem_heap_extra_B=724
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=5
#-----------
time=32258792
mem_heap_B=2710689
mem_heap_extra_B=743
mem_stacks_B=0
heap_tree=detailed
n4: 2710689 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 1572864 0x11036B: void std::vector<char, std::allocator<char> >::_M_range_insert<__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > > >(__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, std::forward_iterator_tag) (new_allocator.h:104)
  n1: 1572864 0x10FBAF: lsys::LSystem::iter(int) (stl_vector.h:1375)
   n1: 1572864 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
    n0: 1572864 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 1062881 0x10FD07: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 1062881 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 1062881 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 72704 0x579E9BE: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.22)
  n1: 72704 0x400F8A8: call_init.part.0 (dl-init.c:72)
   n1: 72704 0x400F9B9: _dl_init (dl-init.c:30)
    n1: 72704 0x4000C58: ??? (in /lib/x86_64-linux-gnu/ld-2.24.so)
     n1: 72704 0x0: ???
      n1: 72704 0xFFF00074D: ???
       n0: 72704 0xFFF00075B: ???
 n0: 2240 in 27 places, all below massif's threshold (1.00%)
#-----------
snapshot=6
#-----------
time=38932440
mem_heap_B=4283553
mem_heap_extra_B=743
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=7
#-----------
time=43520040
mem_heap_B=4283553
mem_heap_extra_B=743
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=8
#-----------
time=49560312
mem_heap_B=4283553
mem_heap_extra_B=743
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=9
#-----------
time=55855808
mem_heap_B=10575009
mem_heap_extra_B=4783
mem_stacks_B=0
heap_tree=detailed
n3: 10575009 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 9437184 0x11036B: void std::vector<char, std::allocator<char> >::_M_range_insert<__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > > >(__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, std::forward_iterator_tag) (new_allocator.h:104)
  n1: 9437184 0x10FBAF: lsys::LSystem::iter(int) (stl_vector.h:1375)
   n1: 9437184 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
    n0: 9437184 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 1062881 0x10FD07: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 1062881 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 1062881 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n0: 74944 in 28 places, all below massif's threshold (1.00%)
#-----------
snapshot=10
#-----------
time=59344864
mem_heap_B=7429276
mem_heap_extra_B=4756
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=11
#-----------
time=65684720
mem_heap_B=9555040
mem_heap_extra_B=4752
mem_stacks_B=0
heap_tree=detailed
n3: 9555040 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 6291456 0x11036B: void std::vector<char, std::allocator<char> >::_M_range_insert<__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > > >(__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, std::forward_iterator_tag) (new_allocator.h:104)
  n1: 6291456 0x10FBAF: lsys::LSystem::iter(int) (stl_vector.h:1375)
   n1: 6291456 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
    n0: 6291456 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 3188645 0x10FD07: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 3188645 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 3188645 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n0: 74939 in 28 places, all below massif's threshold (1.00%)
#-----------
snapshot=12
#-----------
time=72699920
mem_heap_B=9555040
mem_heap_extra_B=4752
mem_stacks_B=0
heap_tree=detailed
n3: 9555040 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 6291456 0x11036B: void std::vector<char, std::allocator<char> >::_M_range_insert<__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > > >(__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, std::forward_iterator_tag) (new_allocator.h:104)
  n1: 6291456 0x10FBAF: lsys::LSystem::iter(int) (stl_vector.h:1375)
   n1: 6291456 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
    n0: 6291456 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 3188645 0x10FD07: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 3188645 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 3188645 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n0: 74939 in 28 places, all below massif's threshold (1.00%)
#-----------
snapshot=13
#-----------
time=81425120
mem_heap_B=9555040
mem_heap_extra_B=4752
mem_stacks_B=0
heap_tree=detailed
n3: 9555040 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 6291456 0x11036B: void std::vector<char, std::allocator<char> >::_M_range_insert<__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > > >(__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, std::forward_iterator_tag) (new_allocator.h:104)
  n1: 6291456 0x10FBAF: lsys::LSystem::iter(int) (stl_vector.h:1375)
   n1: 6291456 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
    n0: 6291456 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 3188645 0x10FD07: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 3188645 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 3188645 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n0: 74939 in 28 places, all below massif's threshold (1.00%)
#-----------
snapshot=14
#-----------
time=90570320
mem_heap_B=9555040
mem_heap_extra_B=4752
mem_stacks_B=0
heap_tree=detailed
n3: 9555040 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 6291456 0x11036B: void std::vector<char, std::allocator<char> >::_M_range_insert<__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > > >(__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, std::forward_iterator_tag) (new_allocator.h:104)
  n1: 6291456 0x10FBAF: lsys::LSystem::iter(int) (stl_vector.h:1375)
   n1: 6291456 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
    n0: 6291456 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 3188645 0x10FD07: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 3188645 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 3188645 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n0: 74939 in 28 places, all below massif's threshold (1.00%)
#-----------
snapshot=15
#-----------
time=95328320
mem_heap_B=9555040
mem_heap_extra_B=4752
mem_stacks_B=0
heap_tree=detailed
n3: 9555040 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 6291456 0x11036B: void std::vector<char, std::allocator<char> >::_M_range_insert<__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > > >(__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, std::forward_iterator_tag) (new_allocator.h:104)
  n1: 6291456 0x10FBAF: lsys::LSystem::iter(int) (stl_vector.h:1375)
   n1: 6291456 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
    n0: 6291456 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 3188645 0x10FD07: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 3188645 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 3188645 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n0: 74939 in 28 places, all below massif's threshold (1.00%)
#-----------
snapshot=16
#-----------
time=100449920
mem_heap_B=9555040
mem_heap_extra_B=4752
mem_stacks_B=0
heap_tree=detailed
n3: 9555040 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 6291456 0x11036B: void std::vector<char, std::allocator<char> >::_M_range_insert<__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > > >(__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, std::forward_iterator_tag) (new_allocator.h:104)
  n1: 6291456 0x10FBAF: lsys::LSystem::iter(int) (stl_vector.h:1375)
   n1: 6291456 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
    n0: 6291456 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 3188645 0x10FD07: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 3188645 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 3188645 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n0: 74939 in 28 places, all below massif's threshold (1.00%)
#-----------
snapshot=17
#-----------
time=106290320
mem_heap_B=9555040
mem_heap_extra_B=4752
mem_stacks_B=0
heap_tree=detailed
n3: 9555040 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 6291456 0x11036B: void std::vector<char, std::allocator<char> >::_M_range_insert<__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > > >(__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, std::forward_iterator_tag) (new_allocator.h:104)
  n1: 6291456 0x10FBAF: lsys::LSystem::iter(int) (stl_vector.h:1375)
   n1: 6291456 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
    n0: 6291456 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 3188645 0x10FD07: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 3188645 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 3188645 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n0: 74939 in 28 places, all below massif's threshold (1.00%)
#-----------
snapshot=18
#-----------
time=113928104
mem_heap_B=9555045
mem_heap_extra_B=4771
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=19
#-----------
time=126515056
mem_heap_B=22137957
mem_heap_extra_B=8811
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=20
#-----------
time=132810552
mem_heap_B=15846501
mem_heap_extra_B=4771
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=21
#-----------
time=140011896
mem_heap_B=15846501
mem_heap_extra_B=4771
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=22
#-----------
time=147688296
mem_heap_B=15846501
mem_heap_extra_B=4771
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=23
#-----------
time=151677096
mem_heap_B=15846501
mem_heap_extra_B=4771
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=24
#-----------
time=159006384
mem_heap_B=15846496
mem_heap_extra_B=4752
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=25
#-----------
time=168574584
mem_heap_B=25412433
mem_heap_extra_B=7015
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=26
#-----------
time=171763248
mem_heap_B=22223788
mem_heap_extra_B=6996
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=27
#-----------
time=181331448
mem_heap_B=31789725
mem_heap_extra_B=9259
mem_stacks_B=0
heap_tree=detailed
n4: 31789725 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 12582912 0x11036B: void std::vector<char, std::allocator<char> >::_M_range_insert<__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > > >(__gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::vector<char, std::allocator<char> > >, std::forward_iterator_tag) (new_allocator.h:104)
  n1: 12582912 0x10FBAF: lsys::LSystem::iter(int) (stl_vector.h:1375)
   n1: 12582912 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
    n0: 12582912 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 9565937 0x10FD07: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 9565937 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 9565937 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 9565937 0x10FE0A: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 9565937 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 9565937 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n0: 74939 in 28 places, all below massif's threshold (1.00%)
#-----------
snapshot=28
#-----------
time=193918400
mem_heap_B=19206813
mem_heap_extra_B=5219
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=29
#-----------
time=201782960
mem_heap_B=21828233
mem_heap_extra_B=5207
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=30
#-----------
time=209651328
mem_heap_B=24449673
mem_heap_extra_B=9239
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=31
#-----------
time=220141128
mem_heap_B=34935433
mem_heap_extra_B=13279
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=32
#-----------
time=225388048
mem_heap_B=29692553
mem_heap_extra_B=9239
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=33
#-----------
time=246363608
mem_heap_B=50664073
mem_heap_extra_B=13279
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=34
#-----------
time=246363608
mem_heap_B=50664073
mem_heap_extra_B=13279
mem_stacks_B=0
heap_tree=detailed
n4: 50664073 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 31457280 0x10E15C: void std::vector<sf::Vertex, std::allocator<sf::Vertex> >::_M_emplace_back_aux<sf::Vertex>(sf::Vertex&&) (in /home/thomas/dev/procgen/procgen.out)
  n1: 31457280 0x10DD3A: logo::go_forward(logo::Turtle const&, logo::Walk&) (in /home/thomas/dev/procgen/procgen.out)
   n1: 31457280 0x10D95F: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
    n0: 31457280 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 9565937 0x10FD07: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 9565937 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 9565937 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 9565937 0x10FE0A: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 9565937 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 9565937 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n0: 74919 in 29 places, all below massif's threshold (1.00%)
#-----------
snapshot=35
#-----------
time=256853408
mem_heap_B=40178313
mem_heap_extra_B=9239
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=36
#-----------
time=298800488
mem_heap_B=82121353
mem_heap_extra_B=13279
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=37
#-----------
time=298800488
mem_heap_B=82121353
mem_heap_extra_B=13279
mem_stacks_B=0
heap_tree=detailed
n4: 82121353 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 62914560 0x10E15C: void std::vector<sf::Vertex, std::allocator<sf::Vertex> >::_M_emplace_back_aux<sf::Vertex>(sf::Vertex&&) (in /home/thomas/dev/procgen/procgen.out)
  n1: 62914560 0x10DD3A: logo::go_forward(logo::Turtle const&, logo::Walk&) (in /home/thomas/dev/procgen/procgen.out)
   n1: 62914560 0x10D95F: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
    n0: 62914560 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 9565937 0x10FD07: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 9565937 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 9565937 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 9565937 0x10FE0A: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 9565937 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 9565937 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n0: 74919 in 29 places, all below massif's threshold (1.00%)
#-----------
snapshot=38
#-----------
time=319776048
mem_heap_B=61149833
mem_heap_extra_B=9239
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=39
#-----------
time=403666168
mem_heap_B=145035913
mem_heap_extra_B=13279
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=40
#-----------
time=403666168
mem_heap_B=145035913
mem_heap_extra_B=13279
mem_stacks_B=0
heap_tree=detailed
n4: 145035913 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 125829120 0x10E15C: void std::vector<sf::Vertex, std::allocator<sf::Vertex> >::_M_emplace_back_aux<sf::Vertex>(sf::Vertex&&) (in /home/thomas/dev/procgen/procgen.out)
  n1: 125829120 0x10DD3A: logo::go_forward(logo::Turtle const&, logo::Walk&) (in /home/thomas/dev/procgen/procgen.out)
   n1: 125829120 0x10D95F: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
    n0: 125829120 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 9565937 0x10FD07: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 9565937 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 9565937 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 9565937 0x10FE0A: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 9565937 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 9565937 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n0: 74919 in 29 places, all below massif's threshold (1.00%)
#-----------
snapshot=41
#-----------
time=445613248
mem_heap_B=103092873
mem_heap_extra_B=9239
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=42
#-----------
time=613389448
mem_heap_B=270865033
mem_heap_extra_B=13279
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=43
#-----------
time=613389448
mem_heap_B=270865033
mem_heap_extra_B=13279
mem_stacks_B=0
heap_tree=detailed
n4: 270865033 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 251658240 0x10E15C: void std::vector<sf::Vertex, std::allocator<sf::Vertex> >::_M_emplace_back_aux<sf::Vertex>(sf::Vertex&&) (in /home/thomas/dev/procgen/procgen.out)
  n1: 251658240 0x10DD3A: logo::go_forward(logo::Turtle const&, logo::Walk&) (in /home/thomas/dev/procgen/procgen.out)
   n1: 251658240 0x10D95F: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
    n0: 251658240 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 9565937 0x10FD07: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 9565937 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 9565937 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 9565937 0x10FE0A: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 9565937 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 9565937 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n0: 74919 in 29 places, all below massif's threshold (1.00%)
#-----------
snapshot=44
#-----------
time=697279568
mem_heap_B=186978953
mem_heap_extra_B=9239
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=45
#-----------
time=792941592
mem_heap_B=282638353
mem_heap_extra_B=11863
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=46
#-----------
time=792941592
mem_heap_B=282638353
mem_heap_extra_B=11863
mem_stacks_B=0
heap_tree=peak
n5: 282638353 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 167772160 0x10E15C: void std::vector<sf::Vertex, std::allocator<sf::Vertex> >::_M_emplace_back_aux<sf::Vertex>(sf::Vertex&&) (in /home/thomas/dev/procgen/procgen.out)
  n1: 167772160 0x10DD3A: logo::go_forward(logo::Turtle const&, logo::Walk&) (in /home/thomas/dev/procgen/procgen.out)
   n1: 167772160 0x10D95F: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
    n0: 167772160 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 95659400 0x10D9D3: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
  n0: 95659400 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 9565937 0x10FD07: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 9565937 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 9565937 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n1: 9565937 0x10FE0A: lsys::LSystem::iter(int) (new_allocator.h:104)
  n1: 9565937 0x10D814: logo::compute_vertices(logo::Turtle const&, int) (in /home/thomas/dev/procgen/procgen.out)
   n0: 9565937 0x10BD8D: main (in /home/thomas/dev/procgen/procgen.out)
 n0: 74919 in 29 places, all below massif's threshold (1.00%)
#-----------
snapshot=47
#-----------
time=802509792
mem_heap_B=273072416
mem_heap_extra_B=9600
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=48
#-----------
time=812077992
mem_heap_B=263506479
mem_heap_extra_B=7337
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=49
#-----------
time=812078072
mem_heap_B=263506434
mem_heap_extra_B=7302
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=50
#-----------
time=812078096
mem_heap_B=263506429
mem_heap_extra_B=7283
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=51
#-----------
time=812078152
mem_heap_B=263506389
mem_heap_extra_B=7267
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=52
#-----------
time=812078208
mem_heap_B=263506349
mem_heap_extra_B=7251
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=53
#-----------
time=812078232
mem_heap_B=263506348
mem_heap_extra_B=7228
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=54
#-----------
time=979854432
mem_heap_B=95734188
mem_heap_extra_B=3188
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=55
#-----------
time=1075516456
mem_heap_B=74788
mem_heap_extra_B=564
mem_stacks_B=0
heap_tree=detailed
n3: 74788 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 72704 0x579E9BE: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.22)
  n1: 72704 0x400F8A8: call_init.part.0 (dl-init.c:72)
   n1: 72704 0x400F9B9: _dl_init (dl-init.c:30)
    n1: 72704 0x4000C58: ??? (in /lib/x86_64-linux-gnu/ld-2.24.so)
     n1: 72704 0x0: ???
      n1: 72704 0xFFF00074D: ???
       n0: 72704 0xFFF00075B: ???
 n0: 1060 in 31 places, all below massif's threshold (1.00%)
 n1: 1024 0x6233A30: _IO_file_doallocate (filedoalloc.c:101)
  n1: 1024 0x6241A44: _IO_doallocbuf (genops.c:398)
   n1: 1024 0x6240DA6: _IO_file_overflow@@GLIBC_2.2.5 (fileops.c:828)
    n1: 1024 0x623FE84: _IO_file_xsputn@@GLIBC_2.2.5 (fileops.c:1339)
     n1: 1024 0x6235079: fwrite (iofwrite.c:39)
      n1: 1024 0x582058C: std::basic_ostream<char, std::char_traits<char> >& std::__ostream_insert<char, std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&, char const*, long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.22)
       n0: 1024 0x10B8B4: main (in /home/thomas/dev/procgen/procgen.out)
#-----------
snapshot=56
#-----------
time=1075516512
mem_heap_B=74740
mem_heap_extra_B=556
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=57
#-----------
time=1075516568
mem_heap_B=74692
mem_heap_extra_B=548
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=58
#-----------
time=1075516624
mem_heap_B=74644
mem_heap_extra_B=540
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=59
#-----------
time=1075516680
mem_heap_B=74596
mem_heap_extra_B=532
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=60
#-----------
time=1075516736
mem_heap_B=74556
mem_heap_extra_B=516
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=61
#-----------
time=1075516760
mem_heap_B=74555
mem_heap_extra_B=493
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=62
#-----------
time=1075516784
mem_heap_B=74550
mem_heap_extra_B=474
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=63
#-----------
time=1075516840
mem_heap_B=74510
mem_heap_extra_B=458
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=64
#-----------
time=1075516864
mem_heap_B=74505
mem_heap_extra_B=439
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=65
#-----------
time=1075516920
mem_heap_B=74465
mem_heap_extra_B=423
mem_stacks_B=0
heap_tree=detailed
n3: 74465 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 72704 0x579E9BE: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.22)
  n1: 72704 0x400F8A8: call_init.part.0 (dl-init.c:72)
   n1: 72704 0x400F9B9: _dl_init (dl-init.c:30)
    n1: 72704 0x4000C58: ??? (in /lib/x86_64-linux-gnu/ld-2.24.so)
     n1: 72704 0x0: ???
      n1: 72704 0xFFF00074D: ???
       n0: 72704 0xFFF00075B: ???
 n1: 1024 0x6233A30: _IO_file_doallocate (filedoalloc.c:101)
  n1: 1024 0x6241A44: _IO_doallocbuf (genops.c:398)
   n1: 1024 0x6240DA6: _IO_file_overflow@@GLIBC_2.2.5 (fileops.c:828)
    n1: 1024 0x623FE84: _IO_file_xsputn@@GLIBC_2.2.5 (fileops.c:1339)
     n1: 1024 0x6235079: fwrite (iofwrite.c:39)
      n1: 1024 0x582058C: std::basic_ostream<char, std::char_traits<char> >& std::__ostream_insert<char, std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&, char const*, long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.22)
       n0: 1024 0x10B8B4: main (in /home/thomas/dev/procgen/procgen.out)
 n0: 737 in 31 places, all below massif's threshold (1.00%)
#-----------
snapshot=66
#-----------
time=1075516976
mem_heap_B=74425
mem_heap_extra_B=407
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=67
#-----------
time=1075517000
mem_heap_B=74424
mem_heap_extra_B=384
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=68
#-----------
time=1075517056
mem_heap_B=74376
mem_heap_extra_B=376
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=69
#-----------
time=1075517112
mem_heap_B=74328
mem_heap_extra_B=368
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=70
#-----------
time=1075517168
mem_heap_B=74280
mem_heap_extra_B=360
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=71
#-----------
time=1075517224
mem_heap_B=74232
mem_heap_extra_B=352
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=72
#-----------
time=1075517280
mem_heap_B=74192
mem_heap_extra_B=336
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=73
#-----------
time=1075517304
mem_heap_B=74191
mem_heap_extra_B=313
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=74
#-----------
time=1075517328
mem_heap_B=74186
mem_heap_extra_B=294
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=75
#-----------
time=1075517384
mem_heap_B=74146
mem_heap_extra_B=278
mem_stacks_B=0
heap_tree=detailed
n3: 74146 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 72704 0x579E9BE: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.22)
  n1: 72704 0x400F8A8: call_init.part.0 (dl-init.c:72)
   n1: 72704 0x400F9B9: _dl_init (dl-init.c:30)
    n1: 72704 0x4000C58: ??? (in /lib/x86_64-linux-gnu/ld-2.24.so)
     n1: 72704 0x0: ???
      n1: 72704 0xFFF00074D: ???
       n0: 72704 0xFFF00075B: ???
 n1: 1024 0x6233A30: _IO_file_doallocate (filedoalloc.c:101)
  n1: 1024 0x6241A44: _IO_doallocbuf (genops.c:398)
   n1: 1024 0x6240DA6: _IO_file_overflow@@GLIBC_2.2.5 (fileops.c:828)
    n1: 1024 0x623FE84: _IO_file_xsputn@@GLIBC_2.2.5 (fileops.c:1339)
     n1: 1024 0x6235079: fwrite (iofwrite.c:39)
      n1: 1024 0x582058C: std::basic_ostream<char, std::char_traits<char> >& std::__ostream_insert<char, std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&, char const*, long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.22)
       n0: 1024 0x10B8B4: main (in /home/thomas/dev/procgen/procgen.out)
 n0: 418 in 31 places, all below massif's threshold (1.00%)
#-----------
snapshot=76
#-----------
time=1075517408
mem_heap_B=74141
mem_heap_extra_B=259
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=77
#-----------
time=1075517464
mem_heap_B=74101
mem_heap_extra_B=243
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=78
#-----------
time=1075517520
mem_heap_B=74061
mem_heap_extra_B=227
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=79
#-----------
time=1075517544
mem_heap_B=74060
mem_heap_extra_B=204
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=80
#-----------
time=1075517600
mem_heap_B=74020
mem_heap_extra_B=188
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=81
#-----------
time=1075517656
mem_heap_B=73980
mem_heap_extra_B=172
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=82
#-----------
time=1075517712
mem_heap_B=73940
mem_heap_extra_B=156
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=83
#-----------
time=1075517768
mem_heap_B=73900
mem_heap_extra_B=140
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=84
#-----------
time=1075517824
mem_heap_B=73860
mem_heap_extra_B=124
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=85
#-----------
time=1075517880
mem_heap_B=73820
mem_heap_extra_B=108
mem_stacks_B=0
heap_tree=detailed
n3: 73820 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n1: 72704 0x579E9BE: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.22)
  n1: 72704 0x400F8A8: call_init.part.0 (dl-init.c:72)
   n1: 72704 0x400F9B9: _dl_init (dl-init.c:30)
    n1: 72704 0x4000C58: ??? (in /lib/x86_64-linux-gnu/ld-2.24.so)
     n1: 72704 0x0: ???
      n1: 72704 0xFFF00074D: ???
       n0: 72704 0xFFF00075B: ???
 n1: 1024 0x6233A30: _IO_file_doallocate (filedoalloc.c:101)
  n1: 1024 0x6241A44: _IO_doallocbuf (genops.c:398)
   n1: 1024 0x6240DA6: _IO_file_overflow@@GLIBC_2.2.5 (fileops.c:828)
    n1: 1024 0x623FE84: _IO_file_xsputn@@GLIBC_2.2.5 (fileops.c:1339)
     n1: 1024 0x6235079: fwrite (iofwrite.c:39)
      n1: 1024 0x582058C: std::basic_ostream<char, std::char_traits<char> >& std::__ostream_insert<char, std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&, char const*, long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.22)
       n0: 1024 0x10B8B4: main (in /home/thomas/dev/procgen/procgen.out)
 n0: 92 in 31 places, all below massif's threshold (1.00%)
#-----------
snapshot=86
#-----------
time=1075517936
mem_heap_B=73780
mem_heap_extra_B=92
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=87
#-----------
time=1075517960
mem_heap_B=73776
mem_heap_extra_B=72
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=88
#-----------
time=1075517984
mem_heap_B=73772
mem_heap_extra_B=52
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=89
#-----------
time=1075518040
mem_heap_B=73732
mem_heap_extra_B=36
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=90
#-----------
time=1075518064
mem_heap_B=73728
mem_heap_extra_B=16
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=91
#-----------
time=1075590776
mem_heap_B=1024
mem_heap_extra_B=8
mem_stacks_B=0
heap_tree=empty
#-----------
snapshot=92
#-----------
time=1075591808
mem_heap_B=0
mem_heap_extra_B=0
mem_stacks_B=0
heap_tree=empty
