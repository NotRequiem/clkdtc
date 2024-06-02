Simple PoC to show how to detect any pattern in autoclicking

# Autoclicker Randomization Detection Pattern

The approach I advocate for detecting autoclickers centers on identifying patterns that occur with such frequency and precision that they are unlikely to be the result of human input. This method minimizes false positives while effectively pinpointing autoclickers, including those employing mathematical algorithms that generate repetitive actions over time. Additionally, it facilitates the detection of click recorders, which inevitably cycle through recorded clicks upon reaching the end of the recorded sequence. Introducing randomization into the process presents another avenue for detection, particularly if the randomization algorithm follows a discernible pattern rather than exhibiting true randomness.

The algorithm proposed for logically discerning patterns in click data operates as follows:

Initially, track the click that it's being used the most (the main click of the player, used to perform common actions) and calculate statistical metrics. These metrics include gini coefficient, serial correlation, shannan entropy, frequency distribution, interquartile Range, mean of the click intervals, median, mode and standard deviation. They are choosed because of its independent nature, as they only rely on data intervals and not in other math formulas that would then become useless to calculate, as we would be calculating new data based on already precalculated data that then would change if the base data is changed.

Once more than **50 clicks** have been registered (a value hardcoded due to the Central Limit Theorem), begin generating vectors of statistics for each click. These vectors encompass the aforementioned statistical metrics and are structured as follows:

`std::vector<double> stats={correlation, entropy, frequency, iqr, mean, median, mode, stdDev}`

For instance, the statistical metrics for click 51 and click 52 would be organized into respective vectors:

```
Stats at click 50: 
Correlation: 0.143725244827598 seconds
Entropy: 0.0107426563383792
Frequency: 0.00011540466520452
IQR Range: 0.664087277829953
Mean: 4.00060216750929
Median: 0.0105528488257754
Mode: 4.85798099512757
Standard Deviation: 1.95771992734835

Stats at click 51:
Correlation: 0.728125244827598 seconds
Entropy: 0.8932426563383792
Frequency: 0.41311540466520452
IQR Range: 0.178987277829953
Mean: 5.91160216750929
Mode: 8.18371992734835
Standard Deviation: 4.1202552448247754
```

Upon reaching a designated buffer size allocated for autoclicker pattern recognition (or, if performance permits, conducting real-time comparisons), a function (designated as 'detect_patterns in my PoC') must compare each member of the statistics vectors with their corresponding counterparts.

Example:

> The function is tasked with systematically comparing the statistical metrics of consecutive click events. Initially, it compares the statistical metrics of click 50 with those of click 51, followed by click 52, click 53, and so forth, extending its comparison to all stored statistical vectors in memory. Once it exhausts the comparison for click 50, it proceeds to repeat the same process for other click events. For instance, upon completing the comparison for click 50, it embarks on comparing the statistical metrics of click 51 with those of click 50, click 52, click 53, click 54, and subsequent clicks.

This comparison entails scrutinizing whether all members of a statistics vector for a particular click closely resemble their corresponding members in other statistics vectors, meaning that, for instance, the standard deviation (stdDev) of the stats vector for click 60 is compared with the standard deviation of the stats vector for click 61, and so forth.

A **threshold of 2** is typically employed for this comparison, adhering to common statistical practices.

Should such similarity be detected across **all members** of a statistics vector in relation to another statistics vector, the function logs the occurrence and the corresponding click counts (rather than time, as users may cease clicking at any juncture). Subsequently, the stored statistics **MUST** be cleared to facilitate the detection of recurring patterns.

For example, if the statistics vector for click 108 closely resembles that of click 279, suggesting a potential autoclicker pattern, the function examines whether the statistics vector for click 279 + 171 (click 450) and those in its vicinity exhibit similar patterns (a range of 5. In this case, 445, 446, 447, 448, 449, 450, 451, 452, 453, 454, 455). Should this recurrence persist over a threshold of **10 instances**, indicative of frequent intervals, the code flags the presence of an autoclicker.

Make sure that you keep track of already found similarties to not analyze patterns on them again if the prediction fails.

This methodology offers a high degree of resilience against false positives. The requirement for two similar vectors across eight distinct statistical measures, occurring at frequent intervals and persisting over a significant number of instances, makes false positives exceedingly rare. Nonetheless, autoclickers operating on hardcoded patterns or formulae, or employing inadequate randomization in the case of click recorders, are prone to detection through this method.
