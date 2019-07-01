# For figure 2B:
# Run program repeatedly and accumulate results in a data file

# group size g = 3
reps <- 200
for (rep in 1:reps) {

system("./EvoProg.exe Data/Run03.inp")

d3 <- read.delim("Data/Run03.txt")
# form "wide" data set, with one variable for each group member
d30 <- subset(d3, select = c("q", "qhat", "w", "R", "theta", "a",
                               "payoff", "delta", "gnum", "inum", "SubPop"))
d3w <- reshape(d30, dir = "wide", idvar = c("gnum", "SubPop"),
                sep = "", timevar = "inum")
av_theta03 <- with(d3w, tapply(theta1, list(q1, q2, q3), mean))
sd_theta03 <- with(d3w, tapply(theta1, list(q1, q2, q3), sd))
tval <- 10 + rep*10
dr <- data.frame(t = tval,
                 av_111 = av_theta03["1", "1", "1"],
                 av_110 = (av_theta03["1", "1", "0"] +
                             av_theta03["1", "0", "1"])/2,
                 av_100 = av_theta03["1", "0", "0"],
                 av_011 = av_theta03["0", "1", "1"],
                 av_010 = (av_theta03["0", "1", "0"] +
                             av_theta03["0", "0", "1"])/2,
                 av_000 = av_theta03["0", "0", "0"],
                 sd_111 = sd_theta03["1", "1", "1"],
                 sd_110 = (sd_theta03["1", "1", "0"] +
                             sd_theta03["1", "0", "1"])/2,
                 sd_100 = sd_theta03["1", "0", "0"],
                 sd_011 = sd_theta03["0", "1", "1"],
                 sd_010 = (sd_theta03["0", "1", "0"] +
                             sd_theta03["0", "0", "1"])/2,
                 sd_000 = sd_theta03["0", "0", "0"])

dr3 <- read.delim("Data/Fig2b_data.txt")
dr3 <- rbind(dr3, dr)
write.table(dr3, "Data/Fig2b_data.txt", quote = FALSE,
            sep = "\t", row.names = FALSE)

}
