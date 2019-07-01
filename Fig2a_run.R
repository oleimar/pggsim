# For figure 2A:
# Run program repeatedly and accumulate results in a data file

# group size g = 2
reps <- 200
for (rep in 1:reps) {

system("./EvoProg.exe Data/Run02.inp")

d2 <- read.delim("Data/Run02.txt")
# form "wide" data set, with one variable for each group member
d20 <- subset(d2, select = c("q", "qhat", "w", "R", "theta", "a",
                               "payoff", "delta", "gnum", "inum", "SubPop"))
d2w <- reshape(d20, dir = "wide", idvar = c("gnum", "SubPop"),
                sep = "", timevar = "inum")
av_theta2 <- with(d2w, tapply(theta1, list(q1, q2), mean))
sd_theta2 <- with(d2w, tapply(theta1, list(q1, q2), sd))
tval <- 500 + rep*10
dr <- data.frame(t = tval,
                 av_11 = av_theta2["1", "1"],
                 av_10 = av_theta2["1", "0"],
                 av_01 = av_theta2["0", "1"],
                 av_00 = av_theta2["0", "0"],
                 sd_11 = sd_theta2["1", "1"],
                 sd_10 = sd_theta2["1", "0"],
                 sd_01 = sd_theta2["0", "1"],
                 sd_00 = sd_theta2["0", "0"])

dr2 <- read.delim("Data/Fig2a_data.txt")
dr2 <- rbind(dr2, dr)
write.table(dr2, "Data/Fig2a_data.txt", quote = FALSE,
            sep = "\t", row.names = FALSE)

}
