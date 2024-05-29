Simple PoC that study what are the best ways to mathematically detect autoclicker randomization. Code not modularized because it's still in development

# Current Stage

Stage 1

# Total PoC Stages

Stage 1: Study statistical and probability algorithms, and make code to calculate these statistical values
Stage 2: Study the best way to collect human clicking patterns and autoclicker patterns
Stage 3: Make the code to detect autoclickers

# Amount of click data necessary to detect autoclickers

> In the case of click recorders, you can't detect them by checking if the data is too random or too consistent like in traditional checks, you need to find repeated clicking patterns. If for example, the user recorded 1k clicks, it will take the anticheat 1k stored clicks for that player in order to see that the recorded clicks are being repeated again.

> In the case of autoclickers with or without randomization, you can detect them by seeing if the clicking behaviour is too random or too consistent. It's theorically possible to flag an autoclicker with 10 clicks, however, most clickers use a math formula to be in a consistent human average (not too random, not too consistent), leading again to having to detect if the player is repeating a click pattern over a certain time or he/she always clicks between a specific but quite short range for a long of time.

In conclusion, there's no specific amount of clicks. The more you can store and analyze in an optimized way, the better.

# Statistics that this code uses in Autoclicker detection
To know why I used them and how they work/how they are calculated, read the code comments in the math folder

Brief explanation of every statistic:
Mean Interval: Average time between consecutive mouse clicks.

Standard Deviation: Measure of the dispersion or spread of the mouse click intervals.

Variance: Measure of how much the mouse click intervals vary from the mean.

Skewness: Measure of asymmetry in the distribution of mouse click intervals.

Kurtosis: Measure of the "tailedness" of the distribution of mouse click intervals.

Average Clicks per Second: Inverse of the mean interval, indicating how many clicks occur in one second on average.

Serial Correlation: Measure of the correlation between successive mouse click intervals.

Entropy: Measure of the randomness or unpredictability of the mouse click interval distribution.

Outliers: Mouse click intervals that are more than 2 standard deviations away from the mean.

Spikes: Large changes between consecutive mouse click intervals.

Covariance between intervals and statistics: Measure of the relationship between mouse click intervals and other statistical properties.

Recurrence Rate, Determinism, Laminarity: Recurrence Quantification Analysis (RQA) measures calculated from recurrence plots derived from mouse click intervals. **This might be changed by Wavelet Analysis or nonlinear measures such as Lyapunov exponent in the future, i have to test.**

# Useless statistics to get in Autoclicker detection
## Rényi entropy or Tsallis entropy
You should use Shannon entropy, as it measures the average amount of information produced by a random variable in a straightforward way. In the context of click intervals, it **directly** quantifies the uncertainty associated with the distribution of intervals, this algorithm is therefore easier to implement and faster to run.

## Outliers
It is not useless, but some anticheat developers use outliers and kurtosis at the same time from external math libraries which doesn't make sense.

You can indirectly calculate outliers with kurtosis. High kurtosis indicates more outliers (heavy tails), while low kurtosis indicates fewer outliers (light tails), like it's being done in the code

You can also identify them with Interquartile Range (IQR), the range between the first quartile (25th percentile) and the third quartile (75th percentile) of the mouse click intervals.

## Benford's Law
Benford's Law states that in many naturally occurring datasets, the leading digits are not uniformly distributed. This is not true at all in many scenarios, where the leading interval frequencies of clicks can be consistent.

## Fast Fourier Transform (Frequency Domain Analysis)
FFT analysis on the click intervals can be used to identify dominant frequencies. Sudden changes or spikes in dominant frequencies may indicate artificial manipulation:

When analyzing click intervals, you can think of them as a time series signal where clicks represent events occurring over time. In the case of autoclicker detection, dominant frequencies might indicate patterns or regularities in clicking behavior. 

You can decompose this signal into its constituent frequencies. The FFT algorithm reveals the relative strength or power of different frequencies within the signal. For example, if an autoclicker operates at a specific frequency or rhythm, such as clicking every 100 milliseconds, you might observe a prominent peak in the frequency spectrum around the corresponding frequency. 

This is useless because it's esentially the same as calculating spikes for a specified average of clicks within a specific standard desviation. The only 'advantage' it could have is allowing you to identify not only spikes but also dominant rhythmic patterns across a range of frequencies (clicking intervals), which we already do with RQA analysis.

## Coefficient of Quartile Variation and Percentiles
CQV is the ratio of the interquartile range to the median, providing a measure of relative variability based on quartiles. Since we're already calculating outliers with kurtosis analysis, which renders calculating a IQR useless due to 25-75% limitations, this formula is also useless.

## Cumulative Distribution Function
CDF can be useful if an autoclicker programatically falls to certain ranges after a specific time below certain thresholds, since it is basically based on plotting the cumulative probability distribution of mouse click intervals. However, most math formulas in autoclickers don't take into account any condition to fall between specific randomization distribution ranges after a specific amount of time or clicks per seconds (let's say that the autoclicker might fall consistently under 11-12 cps after clicking 20cps during 30s to simulate exhaustment), and anyways the RQA analysis would reveal the usage of this pattern if its done over and over.

## Any algorithm based on Time Serial Analysis
Techniques such as moving averages, exponential smoothing, or Fourier analysis to understand temporal patterns in mouse click intervals are already checked with RQA analysis.

## Delta, ranges, modes, maximums and minimums intervals
Most people can click over a very short range (example 11-13 cps) always or with a very large range (5-11cps), or they can click 1cps and then 20cps for fun, this renders any of these kind of statistical values useless.

## Autocorrelation and Detrended Fluctuation Analysis
This is the correlation of the mouse click interval series with itself at different lag values, revealing patterns of correlation at different time delays, which we don't need since it's essentially the same as calculating the covariance between click intervals and click statistics (desviation, entropy, etc).

## Median Interval
This basically is wrongly used by anticheat developers to detect sudden spikes or shifts in the distribution, especially if accompanied by deviations from the median interval, which is the middle value of the sorted list of mouse click intervals. This is done by a lot of legit players naturally as they spike or shift from x mean ranges because a lot of reasons, such as exhaustion or just because they want to. 

## Coefficient of Variation
We just don't need to express the standard deviation as a percentage, it's better to use decimal values

## Fractal Dimensions
This is a bad technique because it detects if the stats of a player are too complex over a lot of time by basically counting the violations of high complexity. It calculates the fractal dimension of the mouse click interval time series using methods like box-counting or correlation dimension, providing insights into the complexity and self-similarity of the data. 

Example: They click with too randomly and with very low cps and very low spikes, then suddenly click consistently, at high cps and without any spike, then they start having a lot of outliers whhen before they were not... 

A player might do this because simply being exhaust or requiring to click in a different way to do something, or even having different states of moods, such as calm, then nervous, etc.
