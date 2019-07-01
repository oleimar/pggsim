# For figure S1 C and D:
# Run program repeatedly and accumulate results in a data file
# Note: the learning rates in the parameter input file need to be adjusted to
# give the different values of lambda (see FigS1AB_data.txt)

# group size g = 3
reps <- 10
for (rep in 1:reps) {

system("./EvoProg.exe Data/Run03_S1.inp")

d3 <- read.delim("Data/Run03_S1.txt")
# form "wide" data set, with one variable for each group member
d30 <- subset(d3, select = c("q", "w", "R", "theta", "a",
                               "delta", "elig", "gnum", "inum", "SubPop"))
d3w <- reshape(d30, dir = "wide", idvar = c("gnum", "SubPop"),
                sep = "", timevar = "inum")
d000 <- subset(d3w, q1 == 0 & q2 == 0 & q3 == 0)
d010 <- subset(d3w, q1 == 0 & q2 == 1 & q3 == 0)
d100 <- subset(d3w, q1 == 1 & q2 == 0 & q3 == 0)
d111 <- subset(d3w, q1 == 1 & q2 == 1 & q3 == 1)

varx000 <- with(d000, var(w1))
vary000 <- with(d000, var(theta1))
corx000 <- with(d000, cor(w1, w2))
cory000 <- with(d000, cor(theta1, theta2))
corxy000 <- with(d000, cor(w1, theta1))

varx010 <- with(d010, var(w1))
vary010 <- with(d010, var(theta1))
corx010 <- with(d010, cor(w1, w2))
cory010 <- with(d010, cor(theta1, theta2))
corxy010 <- with(d010, cor(w1, theta1))

varx100 <- with(d100, var(w1))
vary100 <- with(d100, var(theta1))
corx100 <- with(d100, cor(w1, w2))
cory100 <- with(d100, cor(theta1, theta2))
corxy100 <- with(d100, cor(w1, theta1))

varx111 <- with(d111, var(w1))
vary111 <- with(d111, var(theta1))
corx111 <- with(d111, cor(w1, w2))
cory111 <- with(d111, cor(theta1, theta2))
corxy111 <- with(d111, cor(w1, theta1))

g <- 3
sigma <- 0.05
alphaw <- 0.16
alphatheta <- 0.008
lambda <- 0.9920
dr <- data.frame(g = g, sigma = sigma,
                 alphaw = alphaw, alphatheta = alphatheta, lambda = lambda,
                 varx000 = varx000, vary000 = vary000,
                 corx000 = corx000, cory000 = cory000, corxy000 = corxy000,
                 varx010 = varx010, vary010 = vary010,
                 corx010 = corx010, cory010 = cory010, corxy010 = corxy010,
                 varx100 = varx100, vary100 = vary100,
                 corx100 = corx100, cory100 = cory100, corxy100 = corxy100,
                 varx111 = varx111, vary111 = vary111,
                 corx111 = corx111, cory111 = cory111, corxy111 = corxy111)

dr3 <- read.delim("Data/FigS1CD_data.txt")
dr3 <- rbind(dr3, dr)
write.table(dr3, "Data/FigS1CD_data.txt", quote = FALSE,
            sep = "\t", row.names = FALSE)

}
