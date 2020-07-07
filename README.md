This is a repo of my solution to [Grid Compression Contest](https://www.algotester.com/gcc/en) held by [Algotester](https://algotester.com) and sponsored by [Huawei](https://www.huawei.com/)

Here are [the task](statement.pdf "Grid Compression"), [the final scoreboard](https://algotester.com/en/Contest/ViewScoreboard/60313 "Scoreboard") and my [1st place certificate](certificate.pdf "Certificate").

## Building
The build system is CMake so just run it. If you're not familiar with CMake try reading some [docs](https://cmake.org/cmake/help/latest/manual/cmake.1.html).

## Bootstraping
In order to send your solution to Algotester you have to have a single a file. Here is how you can get it: `sh bootstrap.sh`

## Report
### General idea
The general idea is to convert the grid into an undirected graph and find a [MIS](https://en.wikipedia.org/wiki/Maximal_independent_set "Maximal independent set") in it.

The graph's vertices is the set of rectangles and the edges is the set of pairs of intersecting rectangles. The conversion from grid to graph can be found [here](io.h#L84)

The idea behind finding MIS is the same as in [this paper][1]:
1. Generate random starting solution
1. Improve it with `(1,2)`-swaps and plateau search  
My contribution is the observation that for grids with `(1,x)` or `(x,1)` rectangles (`1<=x<=10`) `(1,2)`-swaps without plateau search always give better results faster.

### Further improvement
- Use reductions akin to [OnlineMIS][2] ([the implementation](https://karlsruhemis.github.io/))
- Find a better plateau search routine

[1]: https://renatowerneck.files.wordpress.com/2016/06/arw10-mis-journal.pdf "D. V. Andrade, M. G. C. Resende, and R. F. Werneck. Fast Local Search for the Maximum Independent Set Problem. Journal of Heuristics, 18(4):525–547, 2012."
[2]: https://arxiv.org/pdf/1602.01659.pdf "Accelerating Local Search for the Maximum Independent Set Problem. Jakob Dahlum, Sebastian Lamm, Peter Sanders, Christian Schulz, Darren Strash and Renato F. Werneck. In Proceedings of the 15th Symposium on Experimental Algorithms (SEA), volume 9685 of LNCS, pages 118--133. Springer, 2016."
