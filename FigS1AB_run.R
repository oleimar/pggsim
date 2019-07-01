# For figure S1 A and B:
# Run program repeatedly and accumulate results in a data file
# Note: the learning rates in the parameter input file need to be adjusted to
# give the different values of lambda (see FigS1AB_data.txt)

# group size g = 2
reps <- 10
for (rep in 1:reps) {

system("./EvoProg.exe Data/Run02_S1.inp")

d2 <- read.delim("Data/Run02_S1.txt")
# form "wide" data set, with one variable for each group member
d20 <- subset(d2, select = c("q", "w", "R", "theta", "a",
                               "delta", "elig", "gnum", "inum", "SubPop"))
d2w <- reshape(d20, dir = "wide", idvar = c("gnum", "SubPop"),
                sep = "", timevar = "inum")
d00 <- subset(d2w, q1 == 0 & q2 == 0)
d01 <- subset(d2w, q1 == 0 & q2 == 1)
d10 <- subset(d2w, q1 == 1 & q2 == 0)
d11 <- subset(d2w, q1 == 1 & q2 == 1)

varx00 <- with(d00, (var(w1) + var(w2))/2)
vary00 <- with(d00, (var(theta1) + var(theta2))/2)
corx00 <- with(d00, cor(w1, w2))
cory00 <- with(d00, cor(theta1, theta2))
corxy00 <- with(d00, (cor(w1, theta1) + cor(w2, theta2))/2)

varx01 <- with(d01, var(w1))
vary01 <- with(d01, var(theta1))
corx01 <- with(d01, cor(w1, w2))
cory01 <- with(d01, cor(theta1, theta2))
corxy01 <- with(d01, cor(w1, theta1))

varx10 <- with(d10, var(w1))
vary10 <- with(d10, var(theta1))
corx10 <- with(d10, cor(w1, w2))
cory10 <- with(d10, cor(theta1, theta2))
corxy10 <- with(d10, cor(w1, theta1))

varx11 <- with(d11, (var(w1) + var(w2))/2)
vary11 <- with(d11, (var(theta1) + var(theta2))/2)
corx11 <- with(d11, cor(w1, w2))
cory11 <- with(d11, cor(theta1, theta2))
corxy11 <- with(d11, (cor(w1, theta1) + cor(w2, theta2))/2)

g <- 2
sigma <- 0.05
alphaw <- 0.16
alphatheta <- 0.008
lambda <- 0.9920
dr <- data.frame(g = g, sigma = sigma,
                 alphaw = alphaw, alphatheta = alphatheta, lambda = lambda,
                 varx00 = varx00, vary00 = vary00,
                 corx00 = corx00, cory00 = cory00, corxy00 = corxy00,
                 varx01 = varx01, vary01 = vary01,
                 corx01 = corx01, cory01 = cory01, corxy01 = corxy01,
                 varx10 = varx10, vary10 = vary10,
                 corx10 = corx10, cory10 = cory10, corxy10 = corxy10,
                 varx11 = varx11, vary11 = vary11,
                 corx11 = corx11, cory11 = cory11, corxy11 = corxy11)

dr2 <- read.delim("Data/FigS1AB_data.txt")
dr2 <- rbind(dr2, dr)
write.table(dr2, "Data/FigS1AB_data.txt", quote = FALSE,
            sep = "\t", row.names = FALSE)

}
