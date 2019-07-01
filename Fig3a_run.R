# For figure 3A:
# Run program repeatedly and accumulate results in a data file

# group size g = 2
reps <- 30
for (rep in 1:reps) {

system("./EvoProg.exe Data/Run12_2.inp")

d12 <- read.delim("Data/Run12_2.txt")
av_qhat <- with(d12, mean(qhat))
sd_qhat <- with(d12, sd(qhat))
av_theta <- with(d12, mean(theta))
sd_theta <- with(d12, sd(theta))
av_w <- with(d12, mean(w))
sd_w <- with(d12, sd(w))
genval <- 1500 + rep*50
dr <- data.frame(gen = genval,
                 av_qhat = av_qhat,
                 av_theta = av_theta,
                 av_w = av_w,
                 sd_qhat = sd_qhat,
                 sd_theta = sd_theta,
                 sd_w = sd_w)

dr12 <- read.delim("Data/Fig3a_data.txt")
dr12 <- rbind(dr12, dr)
write.table(dr12, "Data/Fig3a_data.txt", quote = FALSE,
            sep = "\t", row.names = FALSE)

}
