# For figure 1B:
# Run program repeatedly and accumulate results in a data file

# group size g = 2
reps <- 1000
for (rep in 1:reps) {

system("./EvoProg.exe Data/Run02_1.inp")

d2 <- read.delim("Data/Run02_1.txt")
# form "wide" data set, with one variable for each group member
d20 <- subset(d2, select = c("q", "w", "R", "theta", "a",
                               "gnum", "inum", "SubPop"))
d2w <- reshape(d20, dir = "wide", idvar = c("gnum", "SubPop"),
                sep = "", timevar = "inum")
tval <- 1000 + rep*1
dr <- data.frame(t = tval,
                 w1 = d2w$w1[1],
                 w2 = d2w$w2[1],
                 theta1 = d2w$theta1[1],
                 theta2 = d2w$theta2[1])

dr2 <- read.delim("Data/Fig1b_data.txt")
dr2 <- rbind(dr2, dr)
write.table(dr2, "Data/Fig1b_data.txt", quote = FALSE,
            sep = "\t", row.names = FALSE)

}
